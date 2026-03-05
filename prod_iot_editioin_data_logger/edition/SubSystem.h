#ifndef SubSystem_H
#define SubSystem_H

#include "../core/Timer.h"
#include "./SubGlobals.h"
#include "./SubRS485.h"
#include "./DataLogger.h"

SubRS485 subRS485;
Timer subSysTimer;
DataLogger logger;

class SubSystem {
public:
  SubSystem(void) {}

  void init() {
    configAnalogInputResolution(0);
    subRS485.init();
  }

  void loop() {
    if (subSysTimer.autoTimeout(1000)) {
      logger.log(F("Test data"));
    }
  }

  ~SubSystem() {}
};

#endif