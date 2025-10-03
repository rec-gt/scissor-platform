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

  DigitalOutput relay = digitalOutputs[0];
  DigitalOutput alarm = digitalOutputs[1];
  DigitalOutput powerLight = digitalOutputs[2];
  DigitalOutput warningLight = digitalOutputs[3];
  DigitalOutput trafficGreen = digitalOutputs[4];
  DigitalOutput trafficYellow = digitalOutputs[5];
  DigitalOutput trafficRed = digitalOutputs[6];
public:
  DetectSystem(DigitalInput *digitalInputs, DigitalOutput *digitalOutputs, AnalogInput *analogInputs, AnalogOutput *analogOutputs)
    : digitalInputs(digitalInputs), digitalOutputs(digitalOutputs), analogInputs(analogInputs), analogOutputs(analogOutputs) {}

  void loop() {
    powerLight.connect();
    // trafficLight.listen(sensorManager.getMinDistance());

    if (this->status == SYS_RUNNING) {
      relay.connect();
      alarm.cut();
      warningLight.cut();

      trafficGreen.connect();
      trafficYellow.cut();
      trafficRed.cut();

    } else if (this->status == SYS_STOPPED) {
      relay.cut();
      alarm.connect();
      warningLight.connect();

      trafficGreen.cut();
      trafficYellow.cut();
      trafficRed.connect();

    } else if (this->status == SYS_ALLOW_10S) {
      relay.connect();
      alarm.cut();
      warningLight.connect();

      trafficGreen.connect();
      trafficYellow.connect();
      trafficRed.cut();
    }
  }

  ~DetectSystem() {}
};

#endif