#ifndef SubSystem_H
#define SubSystem_H

#include "./SubGlobals.h"
#include "./SubRS485.h"
#include "./BMSTony.h"
#include "../core/IoT.h"
#include "../core/Timer.h"

SubRS485 subRS485;

BMSTony bms(1);

Timer debouncer;

class SubSystem {
private:
  void handlePublishPayloads() {
    byte dataPoints_1_8 = 0;
    byte dataPoints_9_10 = 0;

    dataPoints_1_8 |= (byte)holdingRegisterValues[0] << 0;
    dataPoints_1_8 |= (byte)holdingRegisterValues[1] << 1;
    dataPoints_1_8 |= (byte)holdingRegisterValues[2] << 2;
    dataPoints_1_8 |= (byte)holdingRegisterValues[3] << 3;
    dataPoints_1_8 |= (byte)holdingRegisterValues[4] << 4;
    dataPoints_1_8 |= (byte)holdingRegisterValues[5] << 5;
    dataPoints_1_8 |= (byte)holdingRegisterValues[6] << 6;
    dataPoints_1_8 |= (byte)holdingRegisterValues[7] << 7;

    dataPoints_9_10 |= (byte)holdingRegisterValues[8] << 0;
    dataPoints_9_10 |= (byte)holdingRegisterValues[9] << 1;

    iot.buildMsg(dataPoints_1_8, dataPoints_9_10, F("[]"), F("[]"));
  }

  void debouncedEventTrigger() {
    if (eventTriggerFlag) {
      if (debouncer.autoTimeout(1000)) {
        Serial.println(F("Event Trigger - Force Publish"));
        iot.forcePublish();
        eventTriggerFlag = false;
      }
    }
  }

public:
  SubSystem(void) {}

  void init() {
    configAnalogInputResolution(0);
    bms.init();
  }

  void loop() {
    bms.loop();
    this->handlePublishPayloads();
    this->debouncedEventTrigger();
  }

  ~SubSystem() {}
};

#endif