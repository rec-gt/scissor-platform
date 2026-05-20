#include "Arduino.h"
#ifndef SubSystem_H
#define SubSystem_H
#define SET_TEMP 60

#include "../core/Globals.h"
#include "./SubGlobals.h"
#include "./SPM.h"
#include "./KPS.h"
#include "./KPSManager.h"

Timer warmUpTimer(5000UL);
Timer sysTimer(10000UL);

SysMonitor sysMonitor;
KPS tempSensors[PARAMETERS_SIZE];
SPM currentReader(247);
KPSManager kpsManager;

DigitalOutput &highTempRelay = digitalOutputs[0];
DigitalOutput &sirenRelay = digitalOutputs[1];

class SubSystem {
private:
  void updateDisplayContent() {
    analogOutputs[0].value = holdingRegisterValues[0];
  }

  void updateMQTTContent() {
    this->overwriteMQTTPayloads();
    iot.buildMsg(DIPayload, DOPayload, AIPayload, AOPayload);
  }

  void overwriteMQTTPayloads() {
    /*=== DI Payload ===*/
    DIPayload = 0;
    DIPayload |= (sysMonitor.isStatus(SUBSYS_STOPPED) ? 1 : 0) << 0;  // DI1
    DIPayload |= (sysMonitor.isHealth(SUBSYS_FAILURE) ? 1 : 0) << 1;  // DI2
    DIPayload |= 0 << 2;
    DIPayload |= 0 << 3;
    DIPayload |= 0 << 4;
    DIPayload |= 0 << 5;
    DIPayload |= 0 << 6;
    DIPayload |= 0 << 7;

    /*=== AI Payload ===*/
    AIPayload = F("[");
    for (size_t i = 0; i < AI_NUMS; i++) {
      AIPayload += analogInputs[i].getValue();
      if (i < AI_NUMS - 1) {
        AIPayload += F(",");
      }
    }
    AIPayload += F("]");

    /*=== AO Payload ===*/
    AOPayload = F("[");
    for (size_t i = 0; i < AO_NUMS; i++) {
      AOPayload += analogOutputs[i].getValue();
      if (i < AO_NUMS - 1) {
        AOPayload += F(",");
      }
    }
    AOPayload += F("]");
  }

public:
  SubSystem(void) {}

  void init() {
    sysMonitor.setStatus(SUBSYS_STOPPED);
    sysMonitor.setHealth(SUBSYS_HEALTHY);

    kpsManager.init();

    warmUpTimer.refresh();
  }

  void loop() {
    /*=== Async Warm Up ===*/
    if (!warmUpTimer.isExpired()) {
      return;
    }

    /*=== Read Current ===*/
    currentReader.loop();

    /*=== Read Temperature Sensors ===*/
    kpsManager.loop();
    kpsManager.debug();


    /*=== Handle Logic ===*/
    if (sysMonitor.isHealth(SUBSYS_FAILURE)) {
      Serial.println(F("SYSTEM FAILURE"));
      highTempRelay.cut();
      sirenRelay.connect();
    } else {
      if (sysMonitor.isStatus(SUBSYS_RUNNING)) {
        Serial.println(F("SUBSYS RUNNING"));
        highTempRelay.connect();
        sirenRelay.cut();

        if (kpsManager.anyOverheat()) {
          sysMonitor.setStatus(SUBSYS_STOPPED);
          iot.forcePublish();  // force publish is required
        }
      }

      if (sysMonitor.isStatus(SUBSYS_STOPPED)) {
        Serial.println(F("SUBSYS_STOPPED"));
        highTempRelay.cut();
        sirenRelay.connect();

        if (kpsManager.allSafe()) {
          sysMonitor.setStatus(SUBSYS_RUNNING);
        }
      }
    }

    this->updateDisplayContent();
    this->updateMQTTContent();
  }

  ~SubSystem() {}
};

#endif