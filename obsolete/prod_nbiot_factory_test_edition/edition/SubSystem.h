#ifndef SubSystem_H
#define SubSystem_H

#include "./Test.h"
#include "./SubGlobals.h"
#include "./SubRS485.h"

SubRS485 subRS485;

Test test;

class SubSystem {
public:
  SubSystem(void) {}

  void init() {
    configAnalogInputResolution(0);
    subRS485.init();
  }

  void loop() {
    test.DO();
    test.AO();
    subRS485.loop();
  }

  ~SubSystem() {}
};

#endif