#ifndef SubSystem_H
#define SubSystem_H

#include "./SubGlobals.h"
#include "./SubRS485.h"
#include "./MBConfig.h"

SubRS485 subRS485;
MBConfig mbConfig;

class SubSystem {
public:
  SubSystem(void) {}

  void init() {
    configAnalogInputResolution(0);
    subRS485.init();
    mbConfig.init();
  }

  void loop() {
    subRS485.loop();
    mbConfig.loop();
  }

  ~SubSystem() {}
};

#endif