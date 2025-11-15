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
  }

  void loop() {
    subModbus.loop();
  }

  ~SubSystem() {}
};

#endif