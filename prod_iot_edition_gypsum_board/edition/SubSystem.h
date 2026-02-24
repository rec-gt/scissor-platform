#ifndef SubSystem_H
#define SubSystem_H

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
    // subRS485.init();
  }

  void loop() {
    s1.loop();
    s2.loop();

    uint16_t r1 = map(340 - constrain(s1.reading, 0, 340), 0, 340, 0, 1023);
    uint16_t r2 = map(340 - constrain(s2.reading, 0, 340), 0, 340, 0, 1023);

    Serial.println(r1);
  }

  ~SubSystem() {}
};

#endif