#ifndef SubSystem_H
#define SubSystem_H

#include "./SubGlobals.h"

class SubSystem {
public:
  SubSystem(void) {}

  void init() {
    configAnalogInputResolution(0);
  }

  void loop() {}

  ~SubSystem() {}
};

#endif