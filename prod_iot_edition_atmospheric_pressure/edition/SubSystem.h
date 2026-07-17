#include <stdint.h>
#ifndef SubSystem_H
#define SubSystem_H

#include "../core/Globals.h"
#include "../core/Timer.h"
#include "./SubGlobals.h"
#include "./SubRS485.h"

Timer subSystemTimer;
SubRS485 subRS485;

AnalogInput &current = analogInputs[0];     // 三相電電流 reading
AnalogInput &ampere500 = analogInputs[11];  // 三相電電流 ampere, 500 = 50.0A

class SubSystem {
private:
  uint16_t ampere = 0;

  void convertToAmpere500() {
    this->ampere = map(constrain(current.getValue(), 0, 1023), 0, 1023, 0, 500);
    ampere500.value = this->ampere;
  }

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
    Serial.println(24 * v1 - 40);
  }

  ~SubSystem() {}
};



#endif