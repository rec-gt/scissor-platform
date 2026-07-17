#include <stdint.h>
#ifndef SubSystem_H
#define SubSystem_H

#include "../core/Globals.h"
#include "../core/Timer.h"
#include "./SubGlobals.h"
#include "./SubRS485.h"


class SubSystem {
private:
  
public:
  SubSystem(void) {
    analogOutputs[1].set(255);  // 拉高AO2，放10V
  }

  void init() {
    configAnalogInputResolution(0);
  }

  void loop() {
    Serial.println(analogInputs[0].getValue());
    Serial.println(analogInputs[1].getValue());
  }

  ~SubSystem() {}
};



#endif