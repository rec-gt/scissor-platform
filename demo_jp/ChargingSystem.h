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
  String recv = "";

  int AT = 2500;          // ambient temp
  int ST = 2500;          // station temp
  int A = 100;            // current
  int SPST = 8000;        // set-point temperature
  int SPA = 500;          // set-point current
  byte C = 1;             // relay cut=0, connect=1
  byte M = MODE_RUNNING;  // system mode
  byte simCache = 0;      // simulation only: problem buffer
  int SIM_AT = 0;
  int SIM_ST = 0;
  int SIM_A = 0;

public:
  ChargingSystem(void){};

  void waitMsg() {
    if (LoRaSerial.available() > 0) {
      char _byte = LoRaSerial.read();

      if (_byte != '\r' && _byte != '\n') {
        this->recv += _byte;
      }

      if (_byte == '\r') {
        this->actionHooks();
        this->pruneRecvBuffer();
        this->pruneSerialBuffer();
      }
    }
  }

  void pruneSerialBuffer() {
    while (LoRaSerial.read() > 0) {};
  }

  void pruneRecvBuffer() {
    this->recv = "";
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
        String newSPAStr = this->recv.substring(idx, idx + 4);
        int newSPA = newSPAStr.toInt();
        this->SPA = newSPA;
      }
    }

    // Running Mode
    {
      int idx = utils.findStrIdx(this->recv, "M:");
      if (idx > -1) {
        String newModeStr = this->recv.substring(idx, idx + 1);
        Serial.println(newModeStr);
        if (newModeStr == "0" || newModeStr == "1" || newModeStr == "2" || newModeStr == "3") {
          int newMode = newModeStr.toInt();
          this->setMode(newMode);
        }
      }
    }

    // ASK request
    {
      int idx = utils.findStrIdx(this->recv, "ASK:");
      if (idx > -1) {
        delay(100);
        this->collectData();
        this->sendStatus();
        delay(100);
      }
    }
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
