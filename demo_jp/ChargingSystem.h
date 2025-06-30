#include "Utils.h"
#include "SystemEnums.h"
#include "Relay.h"
#include "Switch.h"
#include "Thermometer.h"
#include "Ammeter.h"

#ifndef ChargingSystem_h
#define ChargingSystem_h
#define LoRaSerial Serial1

#define SIM_RECOVERED 0
#define SIM_STOPPED_BY_AMBIENT_TEMP 1
#define SIM_STOPPED_BY_STATION_TEMP 2
#define SIM_STOPPED_BY_CURRENT 3

// modules
Relay relay(10);
Thermometer thermometer1(A0);
Thermometer thermometer2(A2);
Ammeter ammeter(A4);

class ChargingSystem {
private:
  byte RECV_BUFFER_SIZE = 4 * 6;
  byte SEND_BUFFER_SIZE = 4 * 7;
  byte recvBuffer[24];

  int AT = 2500;         // ambient temp
  int ST = 2500;         // station temp
  int A = 100;           // current
  int SPT = 8000;       // set-point temperature
  int SPA = 500;         // set-point current
  int C = 1;             // relay cut=0, connect=1
  int M = MODE_RUNNING;  // system mode
  int simCache = 0;      // simulation only: problem recvBuffer
  int SIM_AT = 0;
  int SIM_ST = 0;
  int SIM_A = 0;

public:
  ChargingSystem(void){};
  void waitMsg() {
    if (LoRaSerial.available() >= RECV_BUFFER_SIZE) {
      LoRaSerial.readBytes(recvBuffer, RECV_BUFFER_SIZE);

      this->SPT = recvBuffer[0] | (recvBuffer[1] << 8) | (recvBuffer[2] << 16) | (recvBuffer[3] << 24);
      this->SPA = recvBuffer[4] | (recvBuffer[5] << 8) | (recvBuffer[6] << 16) | (recvBuffer[7] << 24);
      this->M = recvBuffer[8] | (recvBuffer[9] << 8) | (recvBuffer[10] << 16) | (recvBuffer[11] << 24);
      this->SIM_AT = recvBuffer[12] | (recvBuffer[13] << 8) | (recvBuffer[14] << 16) | (recvBuffer[15] << 24);
      this->SIM_ST = recvBuffer[16] | (recvBuffer[17] << 8) | (recvBuffer[18] << 16) | (recvBuffer[19] << 24);
      this->SIM_A = recvBuffer[20] | (recvBuffer[21] << 8) | (recvBuffer[22] << 16) | (recvBuffer[23] << 24);

      // Serial.println(this->SPT);
      // Serial.println(this->SPA);
      // Serial.println(this->M);
      // Serial.println(this->SIM_AT);
      // Serial.println(this->SIM_ST);
      // Serial.println(this->SIM_A);

      this->pruneSerialBuffer();

      this->collectData();
      this->sendStatus();
    }
  }

  void pruneSerialBuffer() {
    while (LoRaSerial.read() > 0) {};
  }

  void collectData() {
    if (this->modeIs(MODE_SIMULATION)) {
      this->AT = this->SIM_AT;
      this->ST = this->SIM_ST;
      this->A = this->SIM_A;

      // error hook
      if (this->AT >= this->SPT) {
        this->simCache |= 1 << SIM_STOPPED_BY_AMBIENT_TEMP;
      }
      if (this->ST >= this->SPT) {
        this->simCache |= 1 << SIM_STOPPED_BY_STATION_TEMP;
      }
      if (this->A >= this->SPA) {
        this->simCache |= 1 << SIM_STOPPED_BY_CURRENT;
      }
      if (this->simCache == SIM_RECOVERED) {
        this->power(true);
      } else {
        this->power(false);

        // error recovery
        if (this->AT < this->SPT) {
          this->simCache &= ~(1 << SIM_STOPPED_BY_AMBIENT_TEMP);
        }
        if (this->ST < this->SPT) {
          this->simCache &= ~(1 << SIM_STOPPED_BY_STATION_TEMP);
        }
        if (this->A < this->SPA) {
          this->simCache &= ~(1 << SIM_STOPPED_BY_CURRENT);
        }
      }
    } else {
      this->AT = thermometer1.get();
      this->ST = thermometer2.get();
      this->A = ammeter.get();

      if (this->modeIs(MODE_RUNNING)) {
        if (this->AT >= this->SPT || this->ST >= this->SPT || this->A >= this->SPA) {
          this->setMode(MODE_STOPPED);
        }
        this->power(true);
      } else if (this->modeIs(MODE_STOPPED)) {
        this->power(false);
      } else if (this->modeIs(MODE_BYPASS)) {
        this->power(true);
      }
    }
  }

  void listen() {
    this->waitMsg();
    thermometer1.listen();
    thermometer2.listen();
    ammeter.listen();
  }

  void sendStatus() {
    String stats = "AT:" + String(this->AT) + ","
                   + "ST:" + String(this->ST) + ","
                   + "A:" + String(this->A) + ","
                   + "SPT:" + String(this->SPT) + ","
                   + "SPA:" + String(this->SPA) + ","
                   + "C:" + String(this->C) + ","
                   + "M:" + String(this->M);

    LoRaSerial.println(stats);
    Serial.println(stats);
  }

  // hardware control / logic
  void setMode(SYSTEM_MODE mode) {
    this->M = mode;
  }

  bool modeIs(SYSTEM_MODE mode) {
    return (this->M == mode);
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
