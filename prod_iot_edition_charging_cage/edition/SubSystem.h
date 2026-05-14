#include "Arduino.h"
#ifndef SubSystem_H
#define SubSystem_H
#define SET_TEMP 60

#include "../core/Globals.h"
#include "./SubGlobals.h"
#include "./KPS.h"

Timer deviceTimer(10000UL);



KPS kps1;
KPS kps2;
KPS kps3;
KPS kps4;
KPS kps5;
KPS kps6;
KPS kps7;
KPS kps8;
KPS kps9;
KPS kps10;
KPS kps11;
KPS kps12;
KPS kps13;
KPS kps14;
KPS kps15;
KPS kps16;

DigitalOutput &powerRelay = digitalOutputs[0];
DigitalOutput &alarmRelay = digitalOutputs[1];
DigitalOutput &commRelay = digitalOutputs[2];

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

  void setChannelNumber(byte num) {
    this->channelNumber = num;
  }

  void isStatus(byte status) {
    return this->sysStatus == status;
  }

  void isHealth(byte health) {
    return this->sysHealth == health;
  }

  void setStatus(byte status) {
    return this->sysStatus = status;
  }

  void setHealth(byte health) {
    return this->sysHealth = health;
  }

  void valueChecker() {
    bool flag = true;  // flag = true 等於系統正常

    for (size_t i = 0; i < this->channelNumber; i++) {
      if (holdingRegisterValues[i] > 2000 * 10) {
        flag = false;
      }
    }

    this->sysHealth = flag ? SUBSYS_HEALTHY : SUBSYS_FAILURE;
  }

  bool spikeFilter() {
    bool flag = true;  // flag = true 等於系統正常

    for (size_t i = 0; i < this->channelNumber; i++) {
      if (holdingRegisterValues[i] > 2000 * 10) {
        flag = false;
      }
    }

    return flag;
  }

  void readTempIn500ms() {
    mbRtuClient.requestFrom(1, HOLDING_REGISTERS, 0, PARAMETERS_SIZE);

    for (size_t i = 0; i < PARAMETERS_SIZE; i++) {
      holdingRegisterValues[i] = (uint32_t)mbRtuClient.read();
    }

    kps1.set(holdingRegisterValues[0]);  // preserved as environment temperature
    kps2.set(holdingRegisterValues[1]);
    kps3.set(holdingRegisterValues[2]);
    kps4.set(holdingRegisterValues[3]);
    kps5.set(holdingRegisterValues[4]);
    kps6.set(holdingRegisterValues[5]);
    kps7.set(holdingRegisterValues[6]);
    kps8.set(holdingRegisterValues[7]);
    kps9.set(holdingRegisterValues[8]);
    kps10.set(holdingRegisterValues[9]);
    kps11.set(holdingRegisterValues[10]);
    kps12.set(holdingRegisterValues[11]);
    kps13.set(holdingRegisterValues[12]);
    kps14.set(holdingRegisterValues[13]);
    kps15.set(holdingRegisterValues[14]);
    kps16.set(holdingRegisterValues[15]);

    for (size_t i = 0; i < PARAMETERS_SIZE; i++) {
      // Serial.println(holdingRegisterValues[i]);
    }
  }

  void monitorCommHealth() {
    byte iotState = iotModuleState + iotConnState + iotMqttMsgState;
    iotState >= 30 ? commRelay.connect() : commRelay.cut();
  }

  void updateDisplayContent() {
    analogInputs[0].value = holdingRegisterValues[0];
    analogInputs[1].value = holdingRegisterValues[1];
    analogInputs[2].value = holdingRegisterValues[2];
    analogInputs[3].value = holdingRegisterValues[3];
    analogInputs[4].value = holdingRegisterValues[4];
    analogInputs[5].value = holdingRegisterValues[5];
  }

  void updateMQTTContent() {
    this->overwriteMQTTPayloads();
    iot.buildMsg(DIPayload, DOPayload, AIPayload, AOPayload);
  }

  void overwriteMQTTPayloads() {
    /*=== DI Payload ===*/
    DIPayload = 0;
    DIPayload |= (sysStatus == SUBSYS_STOPPED ? 1 : 0) << 0;
    DIPayload |= (sysHealth == SUBSYS_FAILURE ? 1 : 0) << 1;
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

    this->setChannelNumber(TARGET_CHANNEL_SIZE);
  }

  void loop() {
    /*=== Read Data ===*/
    if (deviceTimer.autoTimeout(500)) {
      this->readTempIn500ms();
    }

    /*=== Handle Logic ===*/
    if (this->isHealth(SUBSYS_FAILURE)) {
      Serial.println(F("SYSTEM FAILURE"));
      powerRelay.cut();
      alarmRelay.connect();
    } else {
      this->monitorCommHealth();

      if (this->isStatus(SUBSYS_RUNNING)) {
        // logic
        if (kps1.isOverheat(THRESHOLD_DANGEROUS)
            || kps2.isOverheat(THRESHOLD_DANGEROUS)
            || kps3.isOverheat(THRESHOLD_DANGEROUS)
            || kps4.isOverheat(THRESHOLD_DANGEROUS)
            || kps5.isOverheat(THRESHOLD_DANGEROUS)
            || kps6.isOverheat(THRESHOLD_DANGEROUS)) {
          this->sysStatus = SUBSYS_STOPPED;
          iot.forcePublish();  // force publish is required
        }
        Serial.println(F("SUBSYS_RUNNING"));

        // control
        powerRelay.connect();
        alarmRelay.cut();
      }

      if (this->sysStatus == SUBSYS_STOPPED) {
        // logic
        if (kps1.isSafe(THRESHOLD_SAFE)
            && kps2.isSafe(THRESHOLD_SAFE)
            && kps3.isSafe(THRESHOLD_SAFE)
            && kps4.isSafe(THRESHOLD_SAFE)
            && kps5.isSafe(THRESHOLD_SAFE)
            && kps6.isSafe(THRESHOLD_SAFE)) {
          this->sysStatus = SUBSYS_RUNNING;
        }

        Serial.println(F("SUBSYS_STOPPED"));

        // control
        powerRelay.cut();
        alarmRelay.connect();
      }
    }

    this->updateDisplayContent();
    this->updateMQTTContent();
  }


  ~SubSystem() {}
};

#endif