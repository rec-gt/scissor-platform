#include <stdint.h>
#ifndef SubSystem_H
#define SubSystem_H

#include "../core/Globals.h"
#include "../core/Timer.h"
#include "./SubGlobals.h"
#include "./SubRS485.h"

Timer subSystemTimer;
SubRS485 subRS485;

class SubSystem {
private:
  uint16_t ampere = 0;

  void overwriteMQTT() {
    mainSystem.buildPayloads();
    iot.buildMsg(DIPayload, DOPayload, AIPayload, AOPayload);
  }

  bool anyStateChange() {
    bool flag = false;
    for (uint8_t i = 0; i < DI_NUMS; i++) {
      if (digitalInputs[i].hasStateChange()) {
        this->overwriteMQTT();
        flag = true;
      }
    }
    return flag;
  }

public:
  SubSystem(void) {
  }

  void init() {
    configAnalogInputResolution(0);
  }

  void loop() {
    float v1 = (5 / 1023.) * analogInputs[0].getValue();
    float v2 = (5 / 1023.) * analogInputs[1].getValue();
    analogOutputs[0].value = v1 * 100;
    analogOutputs[1].value = v2 * 100;
  }

  ~SubSystem() {}
};



#endif