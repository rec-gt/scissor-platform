#ifndef SubSystem_H
#define SubSystem_H

#include "./SubGlobals.h"
#include "./SubRS485.h"

SubRS485 subRS485;

class SubSystem {
public:
  SubSystem(void) {}

  void init() {
    configAnalogInputResolution(1);
    subRS485.init();
  }

  void loop() {
    subRS485.loop();
  }

  ~SubSystem() {}
};

#endif