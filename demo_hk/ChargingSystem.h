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
Switch modeSwitch(12);


AsyncTimer timer(1000);

class ChargingSystem {
private:
  byte RECV_BUFFER_SIZE = 28;
  byte SEND_BUFFER_SIZE = 4 * 7;

  int AT = 2500;  // ambient temp
  int ST = 2500;  // station temp
  int A = 100;    // current

  int SPT = 6000;       // set-point temperature
  int SPA = 400;        // set-point current
  int SM = SYS_AUTO;    // system mode
  int OM = OP_RUNNING;  // operation status

  bool forcePublishLock = false;

  unsigned long prevMillis = millis();

public:
  ChargingSystem(void){};

  void debug() {
    Serial.print("[");
    Serial.print(this->AT);
    Serial.print(", ");
    Serial.print(this->ST);
    Serial.print(", ");
    Serial.print(this->A);
    Serial.print(", ");
    Serial.print(this->OM);
    Serial.print(", ");
    Serial.print(this->SM);
    Serial.println("]");
  }

  void collectData() {
    this->AT = thermometer1.get();
    this->ST = thermometer2.get();
    this->A = ammeter.get();
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
    nbiot.pubMsgPayload.concat(String(this->OM));
    nbiot.pubMsgPayload.concat("}");

    nbiot.pubMsgPrepare = "AT+QMTPUB=0,0,0,0,rgt/";
    nbiot.pubMsgPrepare.concat(nbiot.IMEI);
    nbiot.pubMsgPrepare.concat("/in,");
    nbiot.pubMsgPrepare.concat(nbiot.pubMsgPayload.length());

    nbiot.pubMsgCommand = nbiot.pubMsgPrepare;
    nbiot.pubMsgCommand.concat(",");
    nbiot.pubMsgCommand.concat(nbiot.pubMsgPayload);
  }

  void handleModeLogic() {
    if (modeSwitch.isOn()) {
      this->SM = SYS_BYPASS;
    } else {
      this->SM = SYS_AUTO;
    }

    switch (this->SM) {
      case SYS_AUTO:
        switch (this->OM) {
          case OP_RUNNING:
            this->forcePublishLock = true;

            chargingRelay.connect();
            alarmRelay.cut();

            if (this->AT >= this->SPT || this->ST >= this->SPT || this->A >= this->SPA) {
              this->OM = OP_STOPPED;
            }
            break;
          case OP_STOPPED:
            chargingRelay.cut();
            alarmRelay.connect();

            if (this->forcePublishLock) {
              nbiot.forcePublish();
              this->forcePublishLock = false;
            }

            if (pressButton.isPressed()) {
              if (this->AT < this->SPT && this->ST < this->SPT && this->A < this->SPA) {
                this->OM = OP_RUNNING;
              }
            }
            break;
        }
        break;

      case SYS_BYPASS:
        chargingRelay.connect();
        alarmRelay.cut();
        break;

      default:
        chargingRelay.cut();
        alarmRelay.cut();
    }
  }

  void listen() {
    pressButton.listen();
    modeSwitch.listen();

    if (millis() - this->prevMillis > 500) {
      thermometer1.listen();
      thermometer2.listen();
      ammeter.listen();

      this->collectData();
      this->preparePublishMsg();
      this->handleModeLogic();
      this->debug();

      this->prevMillis = millis();
    }
  }

  ~ChargingSystem(void){};
};

extern ChargingSystem chargingSystem;

#endif
