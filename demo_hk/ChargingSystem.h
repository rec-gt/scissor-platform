#include "Utils.h"
#include "SystemEnums.h"
#include "Relay.h"
#include "Switch.h"
#include "Thermometer.h"
#include "Ammeter.h"
#include "PressButton.h"

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

  void waitMsg() {
    if (LoRaSerial.available() >= RECV_BUFFER_SIZE) {
      LoRaSerial.readBytes(recvBuffer, RECV_BUFFER_SIZE);

      this->CRC = recvBuffer[24] | (recvBuffer[25] << 8) | (recvBuffer[26] << 16) | (recvBuffer[27] << 24);

      if (!validateCRC(this->CRC)) {
        this->pruneSerialBuffer();
        this->portCanSend = false;
      } else {
        this->SPT = recvBuffer[0] | (recvBuffer[1] << 8) | (recvBuffer[2] << 16) | (recvBuffer[3] << 24);
        this->SPA = recvBuffer[4] | (recvBuffer[5] << 8) | (recvBuffer[6] << 16) | (recvBuffer[7] << 24);
        this->M = recvBuffer[8] | (recvBuffer[9] << 8) | (recvBuffer[10] << 16) | (recvBuffer[11] << 24);
        this->SIM_AT = recvBuffer[12] | (recvBuffer[13] << 8) | (recvBuffer[14] << 16) | (recvBuffer[15] << 24);
        this->SIM_ST = recvBuffer[16] | (recvBuffer[17] << 8) | (recvBuffer[18] << 16) | (recvBuffer[19] << 24);
        this->SIM_A = recvBuffer[20] | (recvBuffer[21] << 8) | (recvBuffer[22] << 16) | (recvBuffer[23] << 24);

        Serial.println(String(this->SPT) + ", " + String(this->SPA) + ", " + String(this->M) + ", " + String(this->SIM_AT) + ", " + String(this->SIM_ST) + ", " + String(this->SIM_A));
        this->portCanSend = true;
        this->pruneSerialBuffer();
        this->sendStatus();
      }
    }
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
    nbiot.pubMsgContent = "{\"csq\":";
    pubMsgContent.concat(nbiot.CSQ);
    pubMsgContent.concat(",");
    pubMsgContent.concat("\"cgatt\":");
    pubMsgContent.concat(nbiot.CGATT);
    pubMsgContent.concat(",");
    pubMsgContent.concat("\"cereg\":\"");
    pubMsgContent.concat(nbiot.CEREG);
    pubMsgContent.concat("\"");
    pubMsgContent.concat(",");
    pubMsgContent.concat("\"din\":");
    pubMsgContent.concat(String(sensorManager.sensorStatusX8));
    pubMsgContent.concat(",");
    pubMsgContent.concat("\"dout\":");
    pubMsgContent.concat(String(sensorManager.sensorStatusX4));
    pubMsgContent.concat("}");

    int contentLen = pubMsgContent.length();

    publishMsgPrepare = "AT+QMTPUB=0,0,0,0,rgt/";
    publishMsgPrepare.concat(nbiot.IMEI);
    publishMsgPrepare.concat("/in,");
    publishMsgPrepare.concat(String(contentLen));

    publishMsgForce = publishMsgPrepare;
    publishMsgForce.concat(",");
    publishMsgForce.concat(pubMsgContent);
  }

  void listen() {
    thermometer1.listen();
    thermometer2.listen();
    ammeter.listen();

    this->collectData();
    this->handleModeChange();
    this->preparePublishMsg();
  }

  void sendStatus() {
    if (!this->portCanSend) { return; }

    int data[8] = { this->AT, this->ST, this->A, this->SPT, this->SPA, this->C, this->M, 37 };
    LoRaSerial.write((byte*)data, sizeof(data));

    String stats = "AT:" + String(this->AT) + ","
                   + "ST:" + String(this->ST) + ","
                   + "A:" + String(this->A) + ","
                   + "SPT:" + String(this->SPT) + ","
                   + "SPA:" + String(this->SPA) + ","
                   + "C:" + String(this->C) + ","
                   + "M:" + String(this->M);
    Serial.println(stats);

    // LoRaSerial.println(stats);
  }

  void setMode(SYSTEM_MODE mode) {
    this->M = mode;
    if (this->modeIs(MODE_RUNNING)) {
      chargingRelay.cut();
      alarmRelay.connect();
    } else if (this->modeIs(MODE_STOPPED)) {
      chargingRelay.connect();
      alarmRelay.cut();
    }
  }

  bool modeIs(SYSTEM_MODE mode) {
    return this->M == mode;
  }

  void power(bool on) {
    if (on) {
      relay.connect();
      this->C = 1;
    } else {
      relay.cut();
      this->C = 0;  // set charge to 0
      this->A = 0;  // pseudo 0 current, prevent throttle
    }
  }

  ~ChargingSystem(void){};
};

extern ChargingSystem chargingSystem;

#endif
