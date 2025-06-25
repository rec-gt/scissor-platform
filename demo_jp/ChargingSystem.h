#ifndef ChargingSystem_h
#define ChargingSystem_h

#define LoRaSerial Serial1

class ChargingSystem {
private:
  String recv = "";
  byte setPointTemp = 80;

public:
  ChargingSystem(){};

  void listenPort() {
    if (LoRaSerial.available()) {
      this->recv = LoRaSerial.readString();
      this->actionHook();
      Serial.println(this->recv);
    }
  }

  void actionHook() {
    this->recv;
  }

  void setSPT() {}

  ~ChargingSystem(){};
};

extern ChargingSystem chargingSystem;

#endif
