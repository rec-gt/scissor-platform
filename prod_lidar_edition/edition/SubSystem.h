#include "../core/DigitalInput.h"
#include "../core/DigitalOutput.h"
#include "../core/AnalogInput.h"
#include "../core/AnalogOutput.h"
#include "../core/NBIoT.h"
#include "../core/DisplayClient.h"
#include "../core/Utils.h"
#include "../core/Globals.h"
#include "./SubModbus485.h"

#ifndef SubSystem_H
#define SubSystem_H

SubModbus485 subModbus485;

class SubSystem {
private:
  uint16_t currMillis = millis();
  uint16_t prevMillis = millis();
  uint16_t currMillis2 = millis();
  uint16_t prevMillis2 = millis();
  DigitalOutput &relay = digitalOutputs[0];

public:
  SubSystem() {
  }

  void init() {}

  void loop() {
    this->currMillis = millis();
    if (this->currMillis - this->prevMillis > 1000) {
      relay.connect();

      this->currMillis2 = millis();
      if (this->currMillis2 - this->prevMillis2 > 1000) {
        this->prevMillis = this->currMillis;
      }
    } else {
      relay.cut();

      this->prevMillis2 = millis();
    }
  }

  ~SubSystem() {}
};

#endif