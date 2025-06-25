#include "Utils.h"
#include "SystemEnums.h"
#include "Relay.h"
#include "Switch.h"

#ifndef ChargingSystem_h
#define ChargingSystem_h
#define LoRaSerial Serial1

// modules
Relay relay(10);
Switch powerSwitch(11);
Switch modeSwitch(12);

class ChargingSystem {
private:
  String recv = "";
  byte setPointTemp = 80;

  float AT = 25;
  float ST = 25;
  float A = 8;
  byte C = 1;
  int SPT = 80;
  byte S = SYS_RUNNING;

  unsigned long sendStatusMillis = millis();
public:
  ChargingSystem(void){};

  // read
  void listenPort() {
    if (LoRaSerial.available()) {
      this->recv = LoRaSerial.readString();
      this->actionHooks();
    }
  }

  void actionHooks() {
    {
      int idx = utils.findStrIdx(this->recv, "STP:");
      if (idx > -1) {
        String newSPTStr = this->recv.substring(idx, idx + 3);
        int newSPT = newSPTStr.toInt();
        this->setSPT(newSPT);
      }
    }
  }

  // control
  void setSPT(int newSPT) {
    this->SPT = newSPT;
  }

  void monitor() {
    powerSwitch.listen();
    modeSwitch.listen();

    if (powerSwitch.isOn()) {
      if (modeSwitch.isOn()) {
        this->S == SYS_RUNNING;
      } else {
        this->S == SYS_BYPASS;
      }
    } else {
      this->S == SYS_STOPPED;
    }


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

  // write
  void collectData() {
    // String AT = String(float(random(800, 900) / 10.0));
    // String ST = String(float(random(800, 900) / 10.0));
    // String A = "0";
    // String C = "0";
    // String SPT = "80";
    // String S = String(SYS_RUNNING);

    this->AT = float(random(230, 270) / 10.0);
    this->ST = float(random(230, 270) / 10.0);
    this->A = float(random(70, 80) / 10.0);
    this->C = 0;
    this->S = SYS_STOPPED;
  }

  void sendStatus() {
    if (millis() - this->sendStatusMillis > 1000) {
      this->collectData();

      String str = "AT:" + String(AT) + "," + "ST:" + String(ST) + "," + "A:" + String(A) + "," + "C:" + String(C) + "," + "SPT:" + String(SPT) + "," + "S:" + String(S);
      LoRaSerial.println(str);
      Serial.println(str);

      this->sendStatusMillis = millis();
    }
  }

  ~ChargingSystem(void){};
};

extern ChargingSystem chargingSystem;

#endif
