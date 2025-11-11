#include "../core/DigitalInput.h"
#include "../core/DigitalOutput.h"
#include "../core/AnalogInput.h"
#include "../core/AnalogOutput.h"
#include "../core/NBIoT.h"
#include "../core/DisplayClient.h"
#include "../core/Utils.h"
#include "../core/Globals.h"
#include "./SubModbus485.h"

#ifndef SubSystem_H
#define SubSystem_H

SubModbus485 subModbus485;

class SubSystem {
private:
  DigitalOutput &relay = digitalOutputs[0];

public:
  SubSystem() {
  }

  void init() {}

  void loop() {
    subModbus485.loop();
  }

  ~SubSystem() {}
};

#endif