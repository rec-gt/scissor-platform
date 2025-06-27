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
Thermometer thermometer1(A4);
Thermometer thermometer2(A2);
Ammeter ammeter(A0);
Switch powerSwitch(11);
Switch modeSwitch(12);

class ChargingSystem {
private:
  String recv = "";

  int AT = 2500;         // ambient temp
  int ST = 2500;         // station temp
  int A = 100;           // current
  byte C = 1;            // relay cut=0, connect=1
  byte SPT = 8000;       // set-point temperature
  byte SPC = 500;        // set-point current
  byte S = SYS_RUNNING;  // system status

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
        String newSPTStr = this->recv.substring(idx, idx + 3);
        int newSPT = newSPTStr.toInt();
        this->SPT = newSPT;
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
      int idx = utils.findStrIdx(this->recv, "S:");
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

    if (this->S == SYS_RUNNING) {
      if (this->AT >= this->SPT || this->ST >= this->SPT) {
        relay.cut();
      } else {
        relay.connect();
      }
    } else if (this->S == SYS_STOPPED) {
      relay.cut();
    } else if (this->S == SYS_BYPASS) {
      relay.connect();
    }
  }

  void sendStatus() {
    if (millis() - this->sendStatusMillis > 1000) {

      String str = "AT:" + String(this->AT) + "," + "ST:" + String(this->ST) + "," + "A:" + String(this->A) + "," + "C:" + String(this->C) + "," + "SPT:" + String(this->SPT) + "," + "S:" + String(this->S);
      LoRaSerial.println(str);
      Serial.println(str);

      this->sendStatusMillis = millis();
    }
  }

  ~ChargingSystem(void){};
};

extern ChargingSystem chargingSystem;

#endif
