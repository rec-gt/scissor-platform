#ifndef SubSystem_H
#define SubSystem_H

#include "./SubGlobals.h"
#include "./SubRS485.h"
#include "./CET.h"

SubRS485 subRS485;
CET cet(46);

class SubSystem {
public:
  SubSystem(void) {}

  void init() {
    configAnalogInputResolution(0);
    cet.init();
  }

  void loop() {
    cet.loop();
    this->handlePublishPayloads();
  }

  ~SubSystem() {}
};

#endif