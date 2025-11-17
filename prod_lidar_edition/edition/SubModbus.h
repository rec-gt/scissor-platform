#include "Arduino.h"
#include "HardwareSerial.h"
#ifndef SubModbus_H
#define SubModbus_H

#include "../core/Globals.h"
#include "./SubGlobals.h"

class SubModbus {
private:
  byte slaveId = 1;
  uint16_t prevMillis = 0;

public:
  bool isDetected = false;

  SubModbus(void) {}

  void
  init() {
    // recommendedStandard485.begin(1200, SERIAL_8N1);
    digitalWrite(RS485_DEFAULT_RE_PIN, HIGH);
    mbRtuClient.begin(1200);
  }

  void loop() {
    this->read();
  }

  void read() {
    uint16_t currMillis = millis();
    if (currMillis - this->prevMillis > 1000) {
      this->prevMillis = currMillis;

      if (!mbRtuClient.requestFrom(this->slaveId, INPUT_REGISTERS, 0, 3)) {
        Serial.println(mbRtuClient.lastError());
      } else {
        Serial.println(mbRtuClient.read());
        this->isDetected = mbRtuClient.read();
        Serial.println(mbRtuClient.read());
      }
    }
  }


  ~SubModbus() {}
};

extern SubModbus subModbus;

#endif