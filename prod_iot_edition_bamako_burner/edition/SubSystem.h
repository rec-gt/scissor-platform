#include "../core/DigitalInput.h"
#include "../core/DigitalOutput.h"
#include "../core/AnalogInput.h"
#include "../core/AnalogOutput.h"
#include "../core/IoT.h"
#include "../core/MainSystem.h"
#include "../core/DisplayClient.h"
#include "../core/Utils.h"
#include "../core/Globals.h"
#include "../core/Timer.h"
#include "./SubGlobals.h"

#ifndef SubSystem_H
#define SubSystem_H

int readingToActualTemp(int reading) {
  return map(constrain(reading, 196, 1023), 196, 1023, 0, 1300);
}

int readingToActualTemp10to1300(int reading) {
  return constrain(map(constrain(reading, 196, 1023), 196, 1023, 0, 1300), 10, 1300);
}

int getAoValue(int actualTemp) {
  // return constrain(map(constrain(actualTemp, 0, 1300), 0, 1300, 0, 255), 0, 255);
  return constrain(map(constrain(actualTemp, 0, 1300), 0, 1300, 0, 255) + (actualTemp * 2 / 100) + 1, 0, 255);  // 第一版，tune過的
}

DigitalInput &running = digitalInputs[0];
DigitalInput &powerStatus = digitalInputs[1];
DigitalInput &fault = digitalInputs[2];
DigitalInput &waterLevelLow = digitalInputs[3];
DigitalInput &waterLevelHigh = digitalInputs[4];

DigitalOutput &relay1 = digitalOutputs[0];  // ?
DigitalOutput &relay2 = digitalOutputs[1];  // ?
DigitalOutput &relay3 = digitalOutputs[2];  // ?
DigitalOutput &relay4 = digitalOutputs[3];  // relay for more than half EP GG

AnalogInput &temp1 = analogInputs[0];
AnalogInput &temp2 = analogInputs[1];
AnalogInput &temp3 = analogInputs[2];
AnalogInput &ev1 = analogInputs[6];
AnalogInput &ev2 = analogInputs[7];
AnalogInput &ev3 = analogInputs[8];
AnalogInput &ev4 = analogInputs[9];
AnalogInput &ev5 = analogInputs[10];
AnalogInput &ev6 = analogInputs[11];

AnalogOutput &ao1 = analogOutputs[0];
AnalogOutput &ao2 = analogOutputs[1];
AnalogOutput &ao3 = analogOutputs[2];

Timer evMonitorTimer;

class SubSystem {
private:
  byte prevPowerStatus = 0;

  void handleTemperature() {
    // int actualTemp1 = readingToActualTemp10to1300(temp1.getValue());
    int actualTemp1 = readingToActualTemp(temp1.getValue());
    int actualTemp2 = readingToActualTemp(temp2.getValue());
    int actualTemp3 = readingToActualTemp(temp3.getValue());
    int aoValue1 = getAoValue(actualTemp1);
    int aoValue2 = getAoValue(actualTemp2);
    int aoValue3 = getAoValue(actualTemp3);

    if (actualTemp1 > 800) {
      relay1.connect();
    } else {
      if (actualTemp1 < 797) {
        relay1.cut();
      }
    }

    if (actualTemp2 > 250) {
      relay2.connect();
    } else {
      if (actualTemp2 < 247) {
        relay2.cut();
      }
    }

    if (actualTemp3 > 30) {
      relay3.connect();
    } else {
      if (actualTemp3 < 27) {
        relay3.cut();
      }
    }

    ao1.set(aoValue1);
    ao2.set(aoValue2);
    ao3.set(aoValue3);
  }

  void handlePowerLossForcePublish() {
    // handle last will for power loss
    byte powerStatusState = powerStatus.getState();
    if (this->prevPowerStatus != powerStatusState) {
      this->prevPowerStatus = powerStatusState;
      if (powerStatusState == 0) {
        mainSystem.buildPayloads();
        mainSystem.handlePublishContent();
        iot.forcePublish();
      }
    };
  }

  void handleEV() {
    // Ensure it is in running mode
    if (running.getState() != HIGH) {  // not running
      evMonitorTimer.refresh();
      relay4.cut();
      return;
    }

    byte threeGG = 0;

    if (ev1.getValue() < 50) {
      threeGG++;
    }
    if (ev2.getValue() < 50) {
      threeGG++;
    }
    if (ev3.getValue() < 50) {
      threeGG++;
    }
    if (ev4.getValue() < 50) {
      threeGG++;
    }
    if (ev5.getValue() < 50) {
      threeGG++;
    }
    if (ev6.getValue() < 50) {
      threeGG++;
    }

    if (threeGG >= 3) {
      if (evMonitorTimer.autoTimeout(300000UL)) {
        relay4.connect();
      }
    } else {
      evMonitorTimer.refresh();
      relay4.cut();
    }
  }

public:
  SubSystem(void) {
    configAnalogInputResolution(0);

    relay1.cut();
    relay2.cut();
    relay3.cut();
    relay4.cut();
  }

  void init() {}

  void loop() {
    running.listen();
    powerStatus.listen();
    fault.listen();
    waterLevelLow.listen();
    waterLevelHigh.listen();

    temp1.listen();
    temp2.listen();
    temp3.listen();

    this->handleTemperature();
    this->handlePowerLossForcePublish();
    this->handleEV();
  }

  ~SubSystem() {}
};

#endif