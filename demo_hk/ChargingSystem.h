#include "Utils.h"
#include "SystemEnums.h"
#include "Relay.h"
#include "Switch.h"
#include "Thermometer.h"
#include "Ammeter.h"
#include "PressButton.h"
#include "NBIoT.h"

#ifndef ChargingSystem_h
#define ChargingSystem_h
#define LoRaSerial Serial1

#define SIM_RECOVERED 0
#define SIM_STOPPED_BY_AMBIENT_TEMP 1
#define SIM_STOPPED_BY_STATION_TEMP 2
#define SIM_STOPPED_BY_CURRENT 3

// modules
Relay relay(10);
Relay alarmRelay(11);
Relay chargingRelay(12);
Thermometer thermometer1(A0);
Thermometer thermometer2(A2);
Ammeter ammeter(A4);
PressButton resetButton(13);

class ChargingSystem {
private:
  byte RECV_BUFFER_SIZE = 28;
  byte SEND_BUFFER_SIZE = 4 * 7;
  byte recvBuffer[28];

  int AT = 2500;  // ambient temp
  int ST = 2500;  // station temp
  int A = 100;    // current

  int SPT = 3000;  // set-point temperature
  int SPA = 250;   // set-point current

  int C = 1;             // relay cut=0, connect=1
  int M = MODE_RUNNING;  // system mode
  int SIM_AT = 2500;
  int SIM_ST = 2500;
  int SIM_A = 1000;

  int CRC = 0;

  bool portCanSend = false;

public:
  ChargingSystem(void){};

  bool validateCRC(int crc) {
    return crc == 37;
  }

  void pruneSerialBuffer() {
    while (LoRaSerial.read() > 0) {};
  }

  void collectData() {
    this->AT = thermometer1.get();
    this->ST = thermometer2.get();
    this->A = ammeter.get();

    Serial.println("===================");
    Serial.println(this->AT);
    Serial.println(this->ST);
    Serial.println(this->A);
  }

  void handleModeChange() {
    if (this->modeIs(MODE_RUNNING)) {
      if (this->AT >= this->SPT || this->ST >= this->SPT || this->A >= this->SPA) {
        this->setMode(MODE_STOPPED);
      }
    } else if (this->modeIs(MODE_STOPPED)) {
      if (resetButton.isPressed()) {
        if (this->AT < this->SPT && this->ST < this->SPT && this->A < this->SPA) {
          this->setMode(MODE_RUNNING);
        }
      }
    }
  }

  void preparePublishMsg() {
    nbiot.pubMsgPayload = "{\"csq\":";
    nbiot.pubMsgPayload.concat(nbiot.CSQ);
    nbiot.pubMsgPayload.concat(",");
    nbiot.pubMsgPayload.concat("\"cgatt\":");
    nbiot.pubMsgPayload.concat(nbiot.CGATT);
    nbiot.pubMsgPayload.concat(",");
    nbiot.pubMsgPayload.concat("\"cereg\":\"");
    nbiot.pubMsgPayload.concat(nbiot.CEREG);
    nbiot.pubMsgPayload.concat("\"");
    nbiot.pubMsgPayload.concat(",");
    nbiot.pubMsgPayload.concat("\"din\":");
    nbiot.pubMsgPayload.concat("255");
    nbiot.pubMsgPayload.concat(",");
    nbiot.pubMsgPayload.concat("\"dout\":");
    nbiot.pubMsgPayload.concat("255");
    nbiot.pubMsgPayload.concat("}");

    nbiot.pubMsgPrepare = "AT+QMTPUB=0,0,0,0,rgt/";
    nbiot.pubMsgPrepare.concat(nbiot.IMEI);
    nbiot.pubMsgPrepare.concat("/in,");
    nbiot.pubMsgPrepare.concat(nbiot.pubMsgPayload.length());

    nbiot.pubMsgCommand = nbiot.pubMsgPrepare;
    nbiot.pubMsgCommand.concat(",");
    nbiot.pubMsgCommand.concat(nbiot.pubMsgPayload);
  }

  void handlePublishMsg() {
  }

  void listen() {
    thermometer1.listen();
    thermometer2.listen();
    ammeter.listen();

    this->collectData();
    this->preparePublishMsg();
    this->handleModeChange();
  }


  void setMode(SYSTEM_MODE mode) {
    this->M = mode;
    if (this->modeIs(MODE_RUNNING)) {
      chargingRelay.connect();
      alarmRelay.cut();
    } else if (this->modeIs(MODE_STOPPED)) {
      nbiot.forcePublish();
      chargingRelay.cut();
      alarmRelay.connect();
    }
  }

  bool modeIs(SYSTEM_MODE mode) {
    return this->M == mode;
  }

  ~ChargingSystem(void){};
};

extern ChargingSystem chargingSystem;

#endif
