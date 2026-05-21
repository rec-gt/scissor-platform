#ifndef SubSystem_H
#define SubSystem_H

#include "../core/Globals.h"
#include "./SubGlobals.h"
#include "./KPS.h"
#include "./KPSManager.h"

SysMonitor sysMonitor;
KPSManager kpsManager;

Timer warmUpTimer(5000UL);

DigitalOutput &powerRelay = digitalOutputs[0];
DigitalOutput &alarmRelay = digitalOutputs[1];
DigitalOutput &commRelay = digitalOutputs[2];

class SubSystem {
private:
  void monitorCommHealth() {
    byte iotState = iotModuleState + iotConnState + iotMqttMsgState;
    iotState >= 30 ? commRelay.connect() : commRelay.cut();
  }

  void updateDisplayContent() {
    for (size_t i = 0; i < PARAMETERS_SIZE - 4; i++) {
      analogInputs[i].value = holdingRegisterValues[i] / 10;
    }
    for (size_t i = 0; i < 4; i++) {
      analogOutputs[i].value = holdingRegisterValues[12 + i] / 10;
    }
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
      AIPayload += (kpsManager.getValue(i) / 10.0);
      if (i < AI_NUMS - 1) {
        AIPayload += F(",");
      }
    }
    AIPayload += F("]");

    /*=== AO Payload ===*/
    AOPayload = F("[");
    for (size_t i = 0; i < AO_NUMS; i++) {
      AOPayload += kpsManager.getValue(12 + i) / 10.0;
      if (i < AO_NUMS - 1) {
        AOPayload += F(",");
      }
    }
    AOPayload += F("]");
  }

public:
  SubSystem(void) {}

  void init() {
    if (!mbRtuClient.begin(9600)) {
      Serial.println(F("Failed to start Modbus RTU Client!"));
      sysMonitor.setHealth(SUBSYS_FAILURE);
    }
    kpsManager.init();
    warmUpTimer.refresh();
  }

  void loop() {
    /*=== Read Temperature ===*/
    kpsManager.loop();

    /*=== Warm Up for Reading===*/
    if (!warmUpTimer.isExpired()) {
      return;
    }

    /*=== Handle Logic ===*/
    if (sysMonitor.isHealth(SUBSYS_FAILURE)) {
      Serial.println(F("SYSTEM FAILURE"));
      powerRelay.cut();
      alarmRelay.connect();
    } else {
      this->monitorCommHealth();

      if (sysMonitor.isStatus(SUBSYS_RUNNING)) {
        Serial.println(F("SUBSYS RUNNING"));
        powerRelay.connect();
        alarmRelay.cut();

        if (kpsManager.anyOverheat()) {
          sysMonitor.setStatus(SUBSYS_STOPPED);
          iot.forcePublish();  // force publish is required
        }
      }

      if (sysMonitor.isStatus(SUBSYS_STOPPED)) {
        Serial.println(F("SUBSYS_STOPPED"));
        powerRelay.cut();
        alarmRelay.connect();

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