#ifndef SubSystem_H
#define SubSystem_H

#include "./SubGlobals.h"
#include "./SubRS485.h"
#include "./CET.h"

SubRS485 subRS485;

CET cet(12);

Timer debouncer;

Toggle eventTriggerFlag;

class SubSystem {
private:
  void handlePublishPayloads() {
    /*=== AI Payload ===*/
    AIPayload = F("[");
    AIPayload += holdingRegisterValues[0];
    AIPayload += F(",");
    AIPayload += holdingRegisterValues[1];
    AIPayload += F(",");
    AIPayload += holdingRegisterValues[2];
    AIPayload += F(",");
    AIPayload += holdingRegisterValues[3];
    AIPayload += F("]");

    iot.buildMsg(0, 0, AIPayload, F("[]"));
  }

  void debouncedEventTrigger() {
    // if (eventTriggerFlag.isReleased()) {
    //   if (debouncer.autoTimeout(1000)) {
    //     Serial.println(F("Event Trigger - Force Publish"));
    //     iot.forcePublish();
    //     eventTriggerFlag.lock();
    //   }
    // }
  }

public:
  SubSystem(void) {}

  void init() {
    configAnalogInputResolution(0);
    cet.init();
  }

  void loop() {
    cet.loop();
    this->handlePublishPayloads();
    this->debouncedEventTrigger();
  }

  ~SubSystem() {}
};

#endif