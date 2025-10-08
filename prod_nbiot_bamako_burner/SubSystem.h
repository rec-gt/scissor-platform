#include "DigitalInput.h"
#include "DigitalOutput.h"
#include "AnalogInput.h"
#include "AnalogOutput.h"
#include "NBIoT.h"
#include "DisplayClient.h"
#include "Utils.h"
#include "Globals.h"

#ifndef SubSystem_H
#define SubSystem_H

class SubSystem {
private:
  DigitalInput *digitalInputs;
  DigitalOutput *digitalOutputs;
  AnalogInput *analogInputs;
  AnalogOutput *analogOutputs;

public:
  SubSystem(DigitalInput *digitalInputs, DigitalOutput *digitalOutputs, AnalogInput *analogInputs, AnalogOutput *analogOutputs)
    : digitalInputs(digitalInputs), digitalOutputs(digitalOutputs), analogInputs(analogInputs), analogOutputs(analogOutputs) {}

  void loop() {
    int reading = analogInputs[0].getValue();
    Serial.println(reading);
    if (reading < 99) {
      analogOutputs[0].set(0);
    } else {
      int actualTemp = map(reading, 99, 504, 0, 1300);
      int displayTemp = map(actualTemp, 0, 1300, 0, 255);
      analogOutputs[0].set(displayTemp);
    }
  }

  ~SubSystem() {}
};

#endif