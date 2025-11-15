#ifndef SubModbus_H
#define SubModbus_H

#include "../core/Globals.h"
#include "./SubGlobals.h"

class SubModbus {
private:
  byte slaveId = 1;
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
    if (millis() - this->prevMillis > 1000) {

      if (!mbRtuClient.requestFrom(this->slaveId, INPUT_REGISTERS, 0, 3)) {
        // Serial.println(mbRtuClient.lastError());
      } else {
        Serial.println(mbRtuClient.read());
        Serial.println(mbRtuClient.read());
        Serial.println(mbRtuClient.read());
      }
    }
  }


  ~SubModbus() {}
};

extern SubModbus subModbus;

#endif