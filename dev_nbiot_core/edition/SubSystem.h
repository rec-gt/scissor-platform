#ifndef SubSystem_H
#define SubSystem_H

#include "./SubGlobals.h"
#include "./RStd485.h"

RStd485 rStd485;

class SubSystem {
public:
  SubSystem(void) {}

  void init() {
    configAnalogInputResolution(0);
    rStd485.init();
  }

  void loop() {
    rStd485.loop();
  }

  ~SubSystem() {}
};

#endif