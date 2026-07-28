#ifndef SubSystem_H
#define SubSystem_H

#include "./SubGlobals.h"
#include "./SubRS485.h"
// #include "./MBConfig.h"
#include "./CET.h"

SubRS485 subRS485;
// MBConfig mbConfig;
CET cet;
class SubSystem {
public:
  SubSystem(void) {}

  void init() {
    configAnalogInputResolution(0);
    subRS485.init();
    // mbConfig.init();
    cet.init();
  }

  void loop() {
    subRS485.loop();
    // mbConfig.loop();
    cet.loop();
  }

  ~SubSystem() {}
};

#endif