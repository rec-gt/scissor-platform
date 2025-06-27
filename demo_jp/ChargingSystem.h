#include "Utils.h"
#include "SystemEnums.h"
#include "Relay.h"
#include "Switch.h"
#include "Thermometer.h"
#include "Ammeter.h"

#ifndef ChargingSystem_h
#define ChargingSystem_h
#define LoRaSerial Serial1

#define SYS_RECOVERED 0
#define SYS_STOPPED_BY_AMBIENT_TEMP 1
#define SYS_STOPPED_BY_STATION_TEMP 2
#define SYS_STOPPED_BY_CURRENT 3

// modules
Relay relay(10);
Thermometer thermometer3(A6);
Thermometer thermometer1(A4);
Thermometer thermometer2(A2);
Ammeter ammeter(A0);

class ChargingSystem {
private:
  String recv = "";

  int AT = 2500;          // ambient temp
  int ST = 2500;          // station temp
  int A = 100;            // current
  int SPST = 8000;        // set-point temperature
  int SPA = 500;          // set-point current
  byte C = 1;             // relay cut=0, connect=1
  byte M = MODE_RUNNING;  // system mode

  byte status = STATUS_RUNNING;  // system status
  byte attachedProblems = 0;     // problem buffer

  unsigned long sendStatusMillis = millis();

public:
  ChargingSystem(void){};

  void waitMsg() {
    if (LoRaSerial.available()) {
      this->recv = LoRaSerial.readString();
      this->actionHooks();
    }
  }

  void actionHooks() {
    // Set point tempareture
    {
      int idx = utils.findStrIdx(this->recv, "SPST:");
      if (idx > -1) {
        String newSPSTStr = this->recv.substring(idx, idx + 4);
        int newSPST = newSPSTStr.toInt();
        this->SPST = newSPST;
      }
    }

    // Set point current
    {
      int idx = utils.findStrIdx(this->recv, "SPA:");
      if (idx > -1) {
      }
    }

    // Running Mode
    {
      int idx = utils.findStrIdx(this->recv, "M:");
      if (idx > -1) {
      }
    }
  }

  void listen() {
    this->waitMsg();

    thermometer1.listen();
    thermometer2.listen();
    ammeter.listen();

    this->AT = thermometer1.get();
    this->ST = thermometer2.get();
    this->A = ammeter.get();

    if (this->M == MODE_RUNNING) {
      // error hook
      if (this->AT >= this->SPST) {
        this->attachedProblems |= 1 << SYS_STOPPED_BY_AMBIENT_TEMP;
      }
      if (this->ST >= this->SPST) {
        this->attachedProblems |= 1 << SYS_STOPPED_BY_STATION_TEMP;
      }
      if (this->A >= this->SPA) {
        this->attachedProblems |= 1 << SYS_STOPPED_BY_CURRENT;
      }

      if (this->attachedProblems == SYS_RECOVERED) {
        relay.connect();
        this->C = 1;
      } else {
        relay.cut();
        this->C = 0;

        // error recovery
        if (this->AT < this->SPST - 50) {
          this->attachedProblems &= ~(1 << SYS_STOPPED_BY_AMBIENT_TEMP);
        }
        if (this->ST < this->SPST - 50) {
          this->attachedProblems &= ~(1 << SYS_STOPPED_BY_STATION_TEMP);
        }
        if (this->A < this->SPA - 5) {
          this->attachedProblems &= ~(1 << SYS_STOPPED_BY_CURRENT);
        }
      }
    } else if (this->M == MODE_STOPPED) {
      relay.cut();
    } else if (this->M == MODE_BYPASS) {
      relay.connect();
    }
  }

  void sendStatus() {
    if (millis() - this->sendStatusMillis > 1000) {

      String stats = "AT:" + String(this->AT) + ","
                     + "ST:" + String(this->ST) + ","
                     + "A:" + String(this->A) + ","
                     + "SPST:" + String(this->SPST) + ","
                     + "SPA:" + String(this->SPA) + ","
                     + "C:" + String(this->C) + ","
                     + "M:" + String(this->M);

      LoRaSerial.println(stats);
      Serial.println(stats);

      this->sendStatusMillis = millis();
    }
  }

  ~ChargingSystem(void){};
};

extern ChargingSystem chargingSystem;

#endif
