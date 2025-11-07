#include <ArduinoRS485.h>
#include <ArduinoModbus.h>
#include "./PowerMeter.h"
#include "./SubGlobals.h"

#ifndef SubSystem_H
#define SubSystem_H

PowerMeter powerMeter(100);

class SubSystem {
public:
  SubSystem(void) {}

  void init() {
    if (!mbClient.begin(9600)) {
      Serial.println(F("Failed to start Modbus RTU Client!"));
      while (1) {};
    }
  }

  void loop() {
    powerMeter.loop();
  }

  ~SubSystem() {}
};

#endif