#ifndef SubSystem_H
#define SubSystem_H

#include "./SubGlobals.h"
#include "./SubModbus.h"
#include "../core/Globals.h"
#include "../core/DigitalOutput.h"

SubModbus subModbus;

class SubSystem {
public:
  SubSystem(void) {}

  void init() {
    configAnalogInputResolution(0);
    subModbus.init();
    relay.cut();
  }

  void loop() {
    subModbus.loop();
    if (subModbus.isDetected) {
      relay.connect();
    } else {
      relay.cut();
    }
  }

  ~SubSystem() {}
};

#endif