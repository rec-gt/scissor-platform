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
  int configurableTemp = 450;

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

  int readingToActualTemp(int reading) {
    if (reading < 99) {
      return 0;
    } else {
      return map(reading, 99, 504, 0, 1300);
    }
  }

  void handle800Temp() {
    int reading = analogInputs[0].getValue();
    int actualTemp = readingToActualTemp(reading);
    int aoValue = map(actualTemp, 0, 1300, 0, 255);

    // === display actual temperature ===
    analogOutputs[0].set(aoValue);

    // === logic control ===
    if (actualTemp > 800) {
      digitalOutputs[0].cut();
    } else {
      digitalOutputs[0].connect();
    }
  }

  void handleConfigurableTemp() {
    int reading = analogInputs[1].getValue();
    int actualTemp = readingToActualTemp(reading);
    int aoValue = map(actualTemp, 0, 1300, 0, 255);

    // === display actual temperature ===
    analogOutputs[1].set(aoValue);

    // === logic control ===
    if (actualTemp > this->configurableTemp) {
      digitalOutputs[1].cut();
    } else {
      digitalOutputs[1].connect();
    }
  }

  ~SubSystem() {}
};

#endif