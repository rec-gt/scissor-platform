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
    s3.loop();
    s4.loop();

    uint16_t reading1 = 1023 - constrain(s1.reading, 0, 1023);
    uint16_t reading2 = 1023 - constrain(s2.reading, 0, 1023);
    uint16_t reading3 = 1023 - constrain(s3.reading, 0, 1023);
    uint16_t reading4 = 1023 - constrain(s4.reading, 0, 1023);

    /*=== Change Display Value ===*/
    analogInputs[0].value = reading1;
    analogInputs[1].value = reading2;
    analogInputs[2].value = reading3;
    analogInputs[3].value = reading4;
    analogInputs[4].value = 0;
    analogInputs[5].value = 0;
    analogInputs[6].value = 0;
    analogInputs[7].value = 0;
    analogInputs[8].value = 0;
    analogInputs[9].value = 0;
    analogInputs[10].value = 0;
    analogInputs[11].value = 0;

    /*=== Change IoT Payload ===*/
    mainSystem.buildPayloads();
    iot.buildMsg(DIPayload, DOPayload, AIPayload, AOPayload);
  }

  ~SubSystem() {}
};

#endif