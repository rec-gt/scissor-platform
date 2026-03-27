#ifndef SubSystem_H
#define SubSystem_H

#include "../core/Timer.h"
#include "./SubGlobals.h"
#include "./SubRS485.h"
#include "./DataLogger.h"

SubRS485 subRS485;
Timer subSysTimer;
DataLogger dataLogger;

class SubSystem {
public:
  SubSystem(void) {}

  void init() {
    configAnalogInputResolution(0);
    subRS485.init();
  }

  void loop() {
    /*=== Register the Data Logger ===*/
    if (subSysTimer.autoTimeout(5000)) {
      Serial.println(mqttPublMsgPayload);
      dataLogger.log(mqttPublMsgPayload);
    }
  }

  ~SubSystem() {}
};

#endif