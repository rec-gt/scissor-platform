#include "Utils.h"
#include "SystemEnums.h"
#include "Relay.h"

#ifndef ChargingSystem_h
#define ChargingSystem_h
#define LoRaSerial Serial1


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

  // modules
  Relay relay(10);

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
    if (this->AT >= this->SPT || this->ST >= this->SPT) {
      this->relay.cut();
    } else {
      this->relay.connect();
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
    this->A = float(random(80, 110) / 10.0);
    this->C = 1;
    this->S = SYS_RUNNING;
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
