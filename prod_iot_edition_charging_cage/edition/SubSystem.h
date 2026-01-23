#ifndef SubSystem_H
#define SubSystem_H

#include "./SubGlobals.h"

int temp1 = 2500;
int temp2 = 2500;
int temp3 = 2500;
int temp4 = 2500;
int temp5 = 2500;
int temp6 = 2500;

DigitalOutput &relay1 = digitalOutputs[0];
DigitalOutput &relay2 = digitalOutputs[1];

AnalogInput &kps1 = analogInputs[0];
AnalogInput &kps2 = analogInputs[1];
AnalogInput &kps3 = analogInputs[2];
AnalogInput &kps4 = analogInputs[3];
AnalogInput &kps5 = analogInputs[4];
AnalogInput &kps6 = analogInputs[5];

class SubSystem {
private:
  int readingToActualTemp(int reading) {
    return map(constrain(reading, 196, 1023), 196, 1023, 0, 1300);
  }

public:
  SubSystem(void) {}

  void init() {
    configAnalogInputResolution(0);
  }

  void loop() {
    kps1.listen();
    kps2.listen();
    kps3.listen();
    kps4.listen();
    kps5.listen();
    kps6.listen();

    temp1 = readingToActualTemp(kps1.getValue());
    temp2 = readingToActualTemp(kps2.getValue());
    temp3 = readingToActualTemp(kps3.getValue());
    temp4 = readingToActualTemp(kps4.getValue());
    temp5 = readingToActualTemp(kps5.getValue());
    temp6 = readingToActualTemp(kps6.getValue());
  }

  ~SubSystem() {}
};

#endif