#ifndef SubSystem_H
#define SubSystem_H

#include "../core/Globals.h"
#include "./SubGlobals.h"
#include "./SubRS485.h"
#include "./Sensor.h"

SubRS485 subRS485;

Sensor s1(A0);
Sensor s2(A1);
Sensor s3(A2);
Sensor s4(A3);

class SubSystem {
public:
  SubSystem(void) {}

  void init() {
    configAnalogInputResolution(0);
  }

  void loop() {
    s1.loop();
    s2.loop();

    uint16_t reading1 = map(340 - constrain(s1.reading, 0, 340), 0, 340, 0, 1023);
    uint16_t reading2 = map(340 - constrain(s2.reading, 0, 340), 0, 340, 0, 1023);

    /*=== Change Display Value ===*/
    analogInputs[0].value = reading1;
    analogInputs[1].value = reading2;

    /*=== Change IoT Payload ===*/
    mainSystem.buildPayloads();
    iot.buildMsg(DIPayload, DOPayload, AIPayload, AOPayload);
  }

  ~SubSystem() {}
};

#endif