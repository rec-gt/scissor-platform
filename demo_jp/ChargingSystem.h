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
  byte BUFFER_SIZE = 4 * 6;

  String recv = "";

  int AT = 2500;         // ambient temp
  int ST = 2500;         // station temp
  int A = 100;           // current
  int SPST = 8000;       // set-point temperature
  int SPA = 500;         // set-point current
  int C = 1;             // relay cut=0, connect=1
  int M = MODE_RUNNING;  // system mode
  byte simCache = 0;     // simulation only: problem buffer
  int SIM_AT = 0;
  int SIM_ST = 0;
  int SIM_A = 0;

public:
  ChargingSystem(void){};
  byte buffer[24];
  void waitMsg() {
    if (LoRaSerial.available() >= BUFFER_SIZE) {
      LoRaSerial.readBytes(buffer, BUFFER_SIZE);

      this->SPST = buffer[0] | (buffer[1] << 8) | (buffer[2] << 16) | (buffer[3] << 24);
      this->SPA = buffer[4] | (buffer[5] << 8) | (buffer[6] << 16) | (buffer[7] << 24);
      this->M = buffer[7] | (buffer[8] << 8) | (buffer[9] << 16) | (buffer[10] << 24);
      this->SIM_AT = buffer[11] | (buffer[12] << 8) | (buffer[13] << 16) | (buffer[14] << 24);
      // this->SIM_ST = buffer[32];
      // this->SIM_A = buffer[64];

      // // Convert the bytes back to integers
      for (int i = 0; i < BUFFER_SIZE; i += 4) {
        int value = 0;
        value = buffer[i] | (buffer[i + 1] << 8) | (buffer[i + 2] << 16) | (buffer[i + 3] << 24);
        Serial.print("Received Integer: ");
        Serial.println(value);
      }

      Serial.println(this->SPST);
      Serial.println(this->SPA);
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
    this->AT = thermometer1.get();
    this->ST = thermometer2.get();
    this->A = ammeter.get();

    if (this->modeIs(MODE_RUNNING)) {
      if (this->AT >= this->SPST || this->ST >= this->SPST || this->A >= this->SPA) {
        this->setMode(MODE_STOPPED);
      }
      this->power(true);
    } else if (this->modeIs(MODE_STOPPED)) {
      this->power(false);
    } else if (this->modeIs(MODE_BYPASS)) {
      this->power(true);
    } else if (this->modeIs(MODE_SIMULATION)) {
      this->AT = this->SIM_AT;
      this->ST = this->SIM_ST;
      this->A = this->SIM_A;

      // error hook
      if (this->AT >= this->SPST) {
        this->simCache |= 1 << SIM_STOPPED_BY_AMBIENT_TEMP;
      }
      if (this->ST >= this->SPST) {
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
        if (this->AT < this->SPST) {
          this->simCache &= ~(1 << SIM_STOPPED_BY_AMBIENT_TEMP);
        }
        if (this->ST < this->SPST) {
          this->simCache &= ~(1 << SIM_STOPPED_BY_STATION_TEMP);
        }
        if (this->A < this->SPA) {
          this->simCache &= ~(1 << SIM_STOPPED_BY_CURRENT);
        }
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
                   + "SPST:" + String(this->SPST) + ","
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
