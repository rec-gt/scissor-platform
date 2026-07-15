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

AnalogInput &current = analogInputs[0];     // 三相電電流 reading
AnalogInput &ampere500 = analogInputs[11];  // 三相電電流 ampere, 500 = 50.0A

class SubSystem {
private:
  uint16_t ampere = 0;

  void convertToAmpere500() {
    this->ampere = map(constrain(current.getValue(), 0, 1023), 0, 1023, 0, 500);
    ampere500.value = this->ampere;
  }

  void overwriteMQTT() {
    mainSystem.buildPayloads();
    iot.buildMsg(DIPayload, DOPayload, AIPayload, AOPayload);
  }

public:
  SubSystem(void) {
    analogOutputs[1].set(255);  // 拉高AO2，放10V
  }

  void init() {
    configAnalogInputResolution(0);
    subRS485.init();
  }

  void loop() {
    subRS485.loop();

    this->convertToAmpere500();

    /*=== if state-change detected ===*/
    for (uint8_t i = 0; i < DI_NUMS; i++) {
      if (digitalInputs[i].hasStateChange()) {
        this->overwriteMQTT();
        iot.forcePublish();
        break;
      }
    }

    /*=== overwrite mqtt payloads in subSystem ===*/
    this->overwriteMQTT();

    /*=== log data locally ===*/
    if (subSystemTimer.autoTimeout(5000)) {
      // Serial.println(mqttPublMsgPayload);
      dataLogger.log(mqttPublMsgPayload);
    }
  }

  ~SubSystem() {}
};



#endif