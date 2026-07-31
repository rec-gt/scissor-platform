#ifndef SubSystem_H
#define SubSystem_H

#include "../core/Globals.h"
#include "./SubGlobals.h"
#include "./SubRS485.h"

SubRS485 subRS485;

DigitalOutput &relay1 = digitalOutputs[0];

class SubSystem {
public:
  SubSystem(void) {}

  void init() {
    configAnalogInputResolution(0);
    subRS485.init();
    relay1.connect();
  }

  void loop() {
    subRS485.loop();
  }

  ~SubSystem() {}
};

#endif