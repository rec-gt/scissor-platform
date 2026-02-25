#ifndef SubSystem_H
#define SubSystem_H

#include "./SubGlobals.h"
#include "./SubRS485.h"
#include "../core/Iot.h"

SubRS485 subRS485;
Timer alarmTimer;

class SubSystem {
public:
  bool canPublish = true;
  byte didata = 0;

  SubSystem(void) {}

  void init() {
    configAnalogInputResolution(0);
    subRS485.init();
  }

  void loop() {
    subRS485.loop();
    if (digitalInputs[0].getState() == 1) {
      if (canPublish) {
        didata |= 1 << 4;
        didata |= 1 << 5;  // X000 1100
        didata |= 0 << 6;
        didata |= 0 << 7;

        iot.buildMsg(didata, 0, F("[]"), F("[]"));
        iot.forcePublish();
        alarmTimer.refresh();
        canPublish = false;
      }
    }

    if (alarmTimer.autoTimeout(5000)) {
      canPublish = true;
    }
  }

  ~SubSystem() {}
};

#endif