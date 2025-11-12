#ifndef SubSystem_H
#define SubSystem_H

#include "./SubGlobals.h"
#include "./PowerMeter.h"

PowerMeter powerMeter(100);

class SubSystem {
public:
  SubSystem(void) {}

  void init() {
    configAnalogInputResolution(0);
    
    if (!mbRtuClient.begin(9600)) {
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