#include "DigitalInput.h"
#include "DigitalOutput.h"
#include "AnalogInput.h"
#include "AnalogOutput.h"
#include "NBIoT.h"
#include "DisplayClient.h"
#include "Utils.h"
#include "Globals.h"

#ifndef DetectSystem_H
#define DetectSystem_H

enum DetectSystemStatus {
  SYS_INIT,
  SYS_STARTING,
  SYS_RUNNING,
  SYS_STOPPED,
  SYS_ALLOW_10S,
  SYS_FAILURE
};

class DetectSystem {
private:
  DigitalInput *digitalInputs;
  DigitalOutput *digitalOutputs;
  AnalogInput *analogInputs;
  AnalogOutput *analogOutputs;
  DetectSystemStatus status;

  uint16_t threshold500 = 500;
  uint16_t threshold800 = 800;

  DigitalOutput relay = digitalOutputs[0];
  DigitalOutput alarm = digitalOutputs[1];
  DigitalOutput powerLight = digitalOutputs[2];
  DigitalOutput warningLight = digitalOutputs[3];
  DigitalOutput trafficGreen = digitalOutputs[4];
  DigitalOutput trafficYellow = digitalOutputs[5];
  DigitalOutput trafficRed = digitalOutputs[6];

  DigitalInput thresholdSwitch = digitalInputs[0];
  DigitalInput pressButton = digitalInputs[1];

  unsigned long tenSecondTimer = 0;

public:
  DetectSystem(DigitalInput *digitalInputs, DigitalOutput *digitalOutputs, AnalogInput *analogInputs, AnalogOutput *analogOutputs)
    : digitalInputs(digitalInputs), digitalOutputs(digitalOutputs), analogInputs(analogInputs), analogOutputs(analogOutputs) {}

  void loop() {
    powerLight.connect();

    if (this->status == SYS_RUNNING) {
      relay.connect();
      alarm.cut();
      warningLight.cut();

      trafficGreen.connect();
      trafficYellow.cut();
      trafficRed.cut();

      if (this->isOneDetected()) {
        this->status = SYS_STOPPED;
      }

    } else if (this->status == SYS_STOPPED) {
      relay.cut();
      alarm.connect();
      warningLight.connect();

      trafficGreen.cut();
      trafficYellow.cut();
      trafficRed.connect();

      if (areAllExcaped()) {
        this->status = SYS_RUNNING;
      }

      if (pressButton.getState() == LOW) {
        this->status = SYS_ALLOW_10S;
        this->tenSecondTimer = millis();
      }

    } else if (this->status == SYS_ALLOW_10S) {
      relay.connect();
      alarm.cut();
      warningLight.connect();

      trafficGreen.connect();
      trafficYellow.connect();
      trafficRed.cut();

      if ((millis() - this->tenSecondTimer) <= 10000) {
        this->status == SYS_STOPPED;
      }
    }
  }

  bool isOneDetected() {
    for (size_t i = 0; i < 10; i++) {
      uint16_t distance = analogInputs[i].value;
      int thresholdDistance = thresholdSwitch.getState() ? this->threshold500 : this->threshold800;
      if (distance <= thresholdDistance) {
        return true;
      }
    }
    return false;
  }

  bool areAllExcaped() {
    bool flag = true;
    for (size_t i = 0; i < 10; i++) {
      uint16_t distance = analogInputs[i].value;
      int thresholdDistance = (thresholdSwitch.getState() ? this->threshold500 : this->threshold800) + 25;
      if (distance >= thresholdDistance) {
        flag = false;
      }
    }
    return flag;
  }

  ~DetectSystem() {}
};

#endif