#ifndef SubSystem_H
#define SubSystem_H

#include "./SubGlobals.h"
#include "./SubRS485.h"
#include "./BMSTony.h"

SubRS485 subRS485;

BMSTony bms(1);

class SubSystem {
public:
  SubSystem(void) {}

  void init() {
    configAnalogInputResolution(0);
    bms.init();
  }

  void loop() {
    bms.loop();
  }

  ~SubSystem() {}
};

#endif