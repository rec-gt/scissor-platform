#include "HardwareSerial.h"
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

enum SubSystemStatus {
  SYS_INIT,
  SYS_STARTING,
  SYS_RUNNING,
  SYS_STOPPED,
  SYS_ALLOW_10S,
  SYS_FAILURE
};

class SubSystem {
private:
  SubSystemStatus status;

  uint16_t threshold500 = 500;
  uint16_t threshold800 = 800;

  DigitalOutput relay = digitalOutputs[0];
  DigitalOutput alarm = digitalOutputs[1];
  DigitalOutput &powerLight = digitalOutputs[2];
  DigitalOutput &warningLight = digitalOutputs[3];
  DigitalOutput &trafficGreen = digitalOutputs[4];
  DigitalOutput &trafficYellow = digitalOutputs[5];
  DigitalOutput &trafficRed = digitalOutputs[6];

  DigitalInput &thresholdSwitch = digitalInputs[0];
  DigitalInput &pressButton = digitalInputs[1];

  unsigned long tenSecondTimer = 0;

public:
  SubSystem() {
    this->status = SYS_RUNNING;
  }

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

      if (areAllEscaped()) {
        this->status = SYS_RUNNING;
      }

      if (pressButton.getState()) {
        this->status = SYS_ALLOW_10S;
        this->tenSecondTimer = millis();
      }
    } else if (this->status == SYS_ALLOW_10S) {
      relay.connect();
      alarm.cut();
      warningLight.cut();

      trafficGreen.connect();
      trafficYellow.connect();
      trafficRed.cut();

      if ((millis() - this->tenSecondTimer) >= 10000) {
        this->status = SYS_STOPPED;
      }
    }

    Serial.println(this->status);
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

  bool areAllEscaped() {
    bool flag = true;
    for (size_t i = 0; i < 10; i++) {
      uint16_t distance = analogInputs[i].value;
      int thresholdDistance = (thresholdSwitch.getState() ? this->threshold500 : this->threshold800) + 25;
      if (distance <= thresholdDistance) {
        flag = false;
      }
    }
    return flag;
  }

  ~SubSystem() {}
};

#endif