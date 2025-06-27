#include "Utils.h"
#include "SystemEnums.h"
#include "Relay.h"
#include "Switch.h"
#include "Thermometer.h"
#include "Ammeter.h"

#ifndef ChargingSystem_h
#define ChargingSystem_h
#define LoRaSerial Serial1

// modules
Relay relay(10);
Thermometer thermometer3(A6);
Thermometer thermometer1(A4);
Thermometer thermometer2(A2);
Ammeter ammeter(A0);
Switch powerSwitch(11);
Switch modeSwitch(12);

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
      int idx = utils.findStrIdx(this->recv, "STP:");
      if (idx > -1) {
        String newSPSTStr = this->recv.substring(idx, idx + 4);
        int newSPST = newSPSTStr.toInt();
        this->SPST = newSPST;
      }
    }

    // Set point current
    {
      int idx = utils.findStrIdx(this->recv, "STC:");
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

    Serial.println(this->AT);

    if (this->M == MODE_RUNNING) {
      Serial.println("Status: " + String(this->status));

      if (this->status == STATUS_RUNNING) {
        relay.connect();
        if ((this->AT >= this->SPST) || (this->ST >= this->SPST) || (this->A >= this->SPA)) {
          this->status = STATUS_STOPPED;
        }
      } else if (this->status == STATUS_STOPPED) {
        relay.cut();
        if ((this->AT < this->SPST - 50) && (this->ST < this->SPST - 50) && (this->A >= this->SPA - 5)) {
          this->status = STATUS_RUNNING;
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
