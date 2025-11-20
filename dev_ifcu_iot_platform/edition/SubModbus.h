#include "Arduino.h"
#include "HardwareSerial.h"
#ifndef SubModbus_H
#define SubModbus_H

#include "../core/Globals.h"
#include "./SubGlobals.h"

class SubModbus {
private:
  byte slaveId = 31;
  uint16_t prevMillis = 0;


public:
  SubModbus(void) {}

  void init() {
    mbRtuClient.begin(9600);
  }

  void loop() {
    this->read();
  }

  void read() {
    uint16_t currMillis = millis();
    if (currMillis - this->prevMillis > 1000) {
      this->prevMillis = currMillis;

      if (!mbRtuClient.requestFrom(this->slaveId, INPUT_REGISTERS, 30000, 16)) {
        Serial.println(mbRtuClient.lastError());
      } else {
        for (size_t i = 0; i < 16; i++) {
          dataStorage[i] = mbRtuClient.read();
        }
      }
    }
  }


  ~SubModbus() {}
};

extern SubModbus subModbus;

#endif