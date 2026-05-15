#include "Arduino.h"
#ifndef SubSystem_H
#define SubSystem_H
#define SET_TEMP 60

#include "../core/Globals.h"
#include "./SubGlobals.h"
#include "./KPS.h"

Timer warmUpTimer(5000UL);
Timer deviceTimer(10000UL);

KPS tempSensors[PARAMETERS_SIZE];

DigitalOutput &powerRelay = digitalOutputs[0];
DigitalOutput &alarmRelay = digitalOutputs[1];
DigitalOutput &commRelay = digitalOutputs[2];

Toggle allSensorSafe;
Toggle anySensorOver;

class SubSystem {
private:
  enum SUBSYS_HEALTH {
    SUBSYS_HEALTHY,
    SUBSYS_FAILURE,
  };

  enum SUBSYS_STATUS {
    SUBSYS_RUNNING,
    SUBSYS_STOPPED,
  };

  byte sysHealth = SUBSYS_HEALTHY;

  byte sysStatus = SUBSYS_RUNNING;

  byte channelNumber = PARAMETERS_SIZE;  // by default 16

  void initChannelNumber(byte num) {
    this->channelNumber = num;
  }

  void initTempSensors() {
    for (byte i = 0; i < this->channelNumber; i++) {
      tempSensors[i] = KPS();
    }
  }

  void setStatus(byte status) {
    this->sysStatus = status;
  }

  void setHealth(byte health) {
    this->sysHealth = health;
  }

  bool isStatus(byte status) {
    return this->sysStatus == status;
  }

  bool isHealth(byte health) {
    return this->sysHealth == health;
  }

  void valueCheck() {
    bool flag = true;  // flag = true 等於系統正常

    for (size_t i = 0; i < this->channelNumber; i++) {
      if (holdingRegisterValues[i] > 2000 * 10) {
        flag = false;
      }
    }

    this->setHealth(flag ? SUBSYS_HEALTHY : SUBSYS_FAILURE);
  }

  void readTempIn1000ms() {
    mbRtuClient.requestFrom(1, HOLDING_REGISTERS, 0, PARAMETERS_SIZE);

    for (size_t i = 0; i < PARAMETERS_SIZE; i++) {
      holdingRegisterValues[i] = (uint32_t)mbRtuClient.read();
      tempSensors[i].set(holdingRegisterValues[i]);
    }

    for (size_t i = 0; i < PARAMETERS_SIZE; i++) {
      Serial.println(holdingRegisterValues[i]);
    }
  }

  void monitorCommHealth() {
    byte iotState = iotModuleState + iotConnState + iotMqttMsgState;
    iotState >= 30 ? commRelay.connect() : commRelay.cut();
  }

  void updateDisplayContent() {
    for (size_t i = 0; i < PARAMETERS_SIZE - 4; i++) {
      // analogInputs[i].value = 23.5;
      analogInputs[i].value = holdingRegisterValues[i] / 10;
    }
    for (size_t i = 0; i < 4; i++) {
      // analogOutputs[i].value = 23.5;
      // analogOutputs[i].value = holdingRegisterValues[12 + i] / 10;
    }
  }

  void updateMQTTContent() {
    this->overwriteMQTTPayloads();
    iot.buildMsg(DIPayload, DOPayload, AIPayload, AOPayload);
  }

  void overwriteMQTTPayloads() {
    /*=== DI Payload ===*/
    DIPayload = 0;
    DIPayload |= (this->isStatus(SUBSYS_STOPPED) ? 1 : 0) << 0;  // DI1
    DIPayload |= (this->isHealth(SUBSYS_FAILURE) ? 1 : 0) << 1;  // DI2
    DIPayload |= 0 << 2;
    DIPayload |= 0 << 3;
    DIPayload |= 0 << 4;
    DIPayload |= 0 << 5;
    DIPayload |= 0 << 6;
    DIPayload |= 0 << 7;

    /*=== DO Payload ===*/
    DOPayload = 0;
    for (size_t i = 0; i < DO_NUMS; i++) {
      DOPayload |= digitalOutputs[i].getState() << i;
    }

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
    if (!mbRtuClient.begin(9600)) {
      Serial.println(F("Failed to start Modbus RTU Client!"));
      this->setHealth(SUBSYS_FAILURE);
    }

    this->initChannelNumber(TARGET_CHANNEL_SIZE);

    allSensorSafe.setToFalse();
    anySensorOver.setToTrue();

    warmUpTimer.refresh();
  }

  void loop() {
    /*=== Async Warm Up ===*/
    if (!warmUpTimer.isExpired()) {
      return;
    }

    /*=== Read Data ===*/
    if (deviceTimer.autoTimeout(500)) {
      this->readTempIn1000ms();
    }

    /*=== Check Data ===*/
    this->valueCheck();

    /*=== Handle Logic ===*/
    if (this->isHealth(SUBSYS_FAILURE)) {
      Serial.println(F("SYSTEM FAILURE"));
      powerRelay.cut();
      alarmRelay.connect();
    } else {
      this->monitorCommHealth();

      if (this->isStatus(SUBSYS_RUNNING)) {
        Serial.println(F("SUBSYS RUNNING"));
        powerRelay.connect();
        alarmRelay.cut();

        // overheat logic
        anySensorOver.setToFalse();

        for (uint8_t i = 0; i < this->channelNumber; i++) {
          if (tempSensors[i].isOverheat(THRESHOLD_DANGEROUS)) {
            anySensorOver.setToTrue();
          };
        }

        if (anySensorOver.isTrue()) {
          this->setStatus(SUBSYS_STOPPED);
          iot.forcePublish();  // force publish is required
        }
      }

      if (this->isStatus(SUBSYS_STOPPED)) {
        Serial.println(F("SUBSYS_STOPPED"));
        powerRelay.cut();
        alarmRelay.connect();

        // recover logic
        allSensorSafe.setToTrue();
        for (uint8_t i = 0; i < this->channelNumber; i++) {
          if (!tempSensors[i].isSafe(THRESHOLD_SAFE)) {
            allSensorSafe.setToFalse();
          };
        }

        if (allSensorSafe.isTrue()) {
          this->setStatus(SUBSYS_RUNNING);
        }
      }
    }

    this->updateDisplayContent();
    this->updateMQTTContent();
  }

  ~SubSystem() {}
};

#endif