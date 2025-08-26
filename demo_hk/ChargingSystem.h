#include "Utils.h"
#include "SystemEnums.h"
#include "Relay.h"
#include "Switch.h"
#include "Thermometer.h"
#include "Ammeter.h"
#include "PressButton.h"
#include "NBIoT.h"
#include "AsyncTimer.h"

#ifndef ChargingSystem_h
#define ChargingSystem_h

#define SIM_RECOVERED 0
#define SIM_STOPPED_BY_AMBIENT_TEMP 1
#define SIM_STOPPED_BY_STATION_TEMP 2
#define SIM_STOPPED_BY_CURRENT 3

Relay chargingRelay(10);
Thermometer thermometer1(A0);
Thermometer thermometer2(A2);
Ammeter ammeter(A4);
PressButton pressButton(30);
Relay alarmRelay(31);

AsyncTimer timer(1000);

class ChargingSystem {
private:
  byte RECV_BUFFER_SIZE = 28;
  byte SEND_BUFFER_SIZE = 4 * 7;
  byte recvBuffer[28];

  int AT = 2500;  // ambient temp
  int ST = 2500;  // station temp
  int A = 100;    // current

  int SPT = 3200;        // set-point temperature
  int SPA = 250;         // set-point current
  int M = MODE_STOPPED;  // system mode

  int C = 1;  // relay cut=0, connect=1
  int SIM_AT = 2500;
  int SIM_ST = 2500;
  int SIM_A = 1000;

  int CRC = 0;

  bool portCanSend = false;

public:
  ChargingSystem(void) {
    this->setMode(MODE_DEFAULT);
  };

  void collectData() {
    this->AT = thermometer1.get();
    this->ST = thermometer2.get();
    this->A = ammeter.get();

    Serial.print("[");
    Serial.print(this->AT);
    Serial.print(", ");
    Serial.print(this->ST);
    Serial.print(", ");
    Serial.print(this->A);
    Serial.print(", ");
    Serial.print(this->M);
    Serial.println("]");
  }

  void handleModeChange() {
    if (this->modeIs(MODE_DEFAULT)) {
      this->setMode(MODE_RUNNING);
    } else if (this->modeIs(MODE_RUNNING)) {
      Serial.println("MODE_RUNNING");
      if (this->AT >= this->SPT || this->ST >= this->SPT || this->A >= this->SPA) {
        this->setMode(MODE_STOPPED);
      }
    } else if (this->modeIs(MODE_STOPPED)) {
      Serial.println("MODE_STOPPED");
      if (pressButton.isPressed()) {
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
    nbiot.pubMsgPayload.concat("\"ain\":");
    nbiot.pubMsgPayload.concat("[");
    nbiot.pubMsgPayload.concat(String(this->AT));
    nbiot.pubMsgPayload.concat(",");
    nbiot.pubMsgPayload.concat(String(this->ST));
    nbiot.pubMsgPayload.concat(",");
    nbiot.pubMsgPayload.concat(String(this->A));
    nbiot.pubMsgPayload.concat("]");
    nbiot.pubMsgPayload.concat(",");
    nbiot.pubMsgPayload.concat("\"din\":");
    nbiot.pubMsgPayload.concat(String(this->M));
    nbiot.pubMsgPayload.concat("}");

    nbiot.pubMsgPrepare = "AT+QMTPUB=0,0,0,0,rgt/";
    nbiot.pubMsgPrepare.concat(nbiot.IMEI);
    nbiot.pubMsgPrepare.concat("/in,");
    nbiot.pubMsgPrepare.concat(nbiot.pubMsgPayload.length());

    nbiot.pubMsgCommand = nbiot.pubMsgPrepare;
    nbiot.pubMsgCommand.concat(",");
    nbiot.pubMsgCommand.concat(nbiot.pubMsgPayload);
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
    } else if (this->modeIs(MODE_DEFAULT)) {
      chargingRelay.cut();
      alarmRelay.cut();
    }
  }

  bool modeIs(SYSTEM_MODE mode) {
    return this->M == mode;
  }

  void listen() {
    pressButton.listen();

    if (timer.autoExpired(1000)) {
      thermometer1.listen();
      thermometer2.listen();
      ammeter.listen();

      this->collectData();
      this->preparePublishMsg();
      this->handleModeChange();
    }
  }

  ~ChargingSystem(void){};
};

extern ChargingSystem chargingSystem;

#endif
