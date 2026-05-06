#ifndef SubSystem_H
#define SubSystem_H

#include "./SubGlobals.h"
#include "./SubRS485.h"
#include "./CET.h"

SubRS485 subRS485;
CET cet(1);

class SubSystem {
public:
  SubSystem(void) {}

  void init() {
    configAnalogInputResolution(0);
    subRS485.init();
    cet.init();
  }

  void loop() {
    subRS485.loop();
    cet.loop();
  }

  ~SubSystem() {}
};

#endif