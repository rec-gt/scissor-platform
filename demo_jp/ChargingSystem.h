#include "Utils.h"
#include "SystemEnums.h"

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
  byte SPT = 80;
  byte S = SYS_RUNNING;

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

  void setSPT(int newSPT) {
    this->SPT = newSPT;
  }

  // write
  void sendStatus() {
    this->AT = float(random(230, 270) / 10.0);
    this->ST = float(random(230, 270) / 10.0);
    this->A = float(random(80, 110) / 10.0);
    this->C = 1;
    this->S = SYS_RUNNING;

    String str = "AT:" + String(AT) + "," + "ST:" + String(ST) + "," + "A:" + String(A) + "," + "C:" + String(C) + "," + "SPT:" + String(SPT) + "," + "S:" + String(S);
    LoRaSerial.println(str);
    Serial.println(str);
  }

  ~ChargingSystem(void){};
};

extern ChargingSystem chargingSystem;

#endif
