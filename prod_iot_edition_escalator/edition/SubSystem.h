#include <stdint.h>
#ifndef SubSystem_H
#define SubSystem_H

#include "../core/Globals.h"
#include "../core/Timer.h"
#include "./SubGlobals.h"
#include "./SubRS485.h"
#include "./DataLogger.h"

Timer subSystemTimer;
SubRS485 subRS485;

DigitalInput &mainPower = digitalInputs[0];  // 市電
DigitalInput &di2 = digitalInputs[1];        // spare or depends on application
DigitalInput &di3 = digitalInputs[2];        // spare or depends on application
DigitalInput &di4 = digitalInputs[3];        // spare or depends on application
DigitalInput &di5 = digitalInputs[4];        // spare or depends on application
DigitalInput &di6 = digitalInputs[5];        // spare or depends on application
DigitalInput &di7 = digitalInputs[6];        // spare or depends on application
DigitalInput &di8 = digitalInputs[7];        // spare or depends on application

AnalogInput &current = analogInputs[0];      // 三相電電流 reading
AnalogInput &ampere1000 = analogInputs[11];  // 三相電電流 ampere


class SubSystem {
private:
  uint16_t ampere = 0;

  void convertToAmpere1000() {
    this->ampere = map(constrain(current.getValue(), 196, 996), 196, 996, 0, 1000);
    ampere1000.value = this->ampere;
  }

public:
  SubSystem(void) {}

  void init() {
    configAnalogInputResolution(0);
    subRS485.init();
  }

  void loop() {
    subRS485.loop();

    this->convertToAmpere1000();

    /*=== overwrite mqtt payloads in subSystem ===*/
    mainSystem.buildPayloads();
    iot.buildMsg(DIPayload, DOPayload, AIPayload, AOPayload);

    /*=== log data locally ===*/
    if (subSystemTimer.autoTimeout(5000)) {
      Serial.println(mqttPublMsgPayload);
      dataLogger.log(mqttPublMsgPayload);
    }
  }

  ~SubSystem() {}
};



#endif