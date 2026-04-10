#include "Arduino.h"
#include "../core/DigitalInput.h"
#include "../core/DigitalOutput.h"
#include "../core/AnalogInput.h"
#include "../core/AnalogOutput.h"
#include "../core/IoT.h"
#include "../core/DisplayClient.h"
#include "../core/Utils.h"
#include "../core/Globals.h"
#include "./SubGlobals.h"
#include "./SubRS485.h"

#ifndef SubSystem_H
#define SubSystem_H

SubRS485 rStd485;

enum SubSystemStatus {
  SYS_INIT,
  SYS_STARTING,
  SYS_RUNNING,
  SYS_STOPPED,
  SYS_ALLOW_10S,
  SYS_FAILURE
};

Timer alarmTimer;
Timer triggerTimer;

class SubSystem {
private:
  SubSystemStatus status;
  byte prevStatus = SYS_RUNNING;

  enum THRESHOLD_DISTANCE {
    ALL_600,
    ALL_800,
    ALL_1000,
    ALL_1200,
    ALL_1400,
  };

  byte thresholdDistance = ALL_1000;

  DigitalOutput &relay = digitalOutputs[0];
  DigitalOutput &alarm = digitalOutputs[1];
  DigitalOutput &powerLight = digitalOutputs[2];
  DigitalOutput &warningLight = digitalOutputs[3];
  DigitalOutput &trafficGreen = digitalOutputs[4];
  DigitalOutput &trafficYellow = digitalOutputs[5];
  DigitalOutput &trafficRed = digitalOutputs[6];

  // DigitalInput &thresholdSwitch = digitalInputs[0]; // deprecated
  DigitalInput &pressButton = digitalInputs[1];

  unsigned long tenSecondTimer = 0;

  void forceInitEEP() {
    EEPROM.put(EEP_ADDR_THRESHOLD_DISTANCE, 12);
    EEPROM.put(EEP_ADDR_ESCAPE_COUNT_DOWN, 10);
    EEPROM.put(EEP_ADDR_TRIGGER_DURATION, 5);
  }

  void initThresholdDistance() {
    int prevSensorThresholdDistance = 0;
    prevSensorThresholdDistance = EEPROM.read(EEP_ADDR_THRESHOLD_DISTANCE);
    if (prevSensorThresholdDistance == 6 || prevSensorThresholdDistance == 8 || prevSensorThresholdDistance == 10 || prevSensorThresholdDistance == 12) {
      sensorThresholdDistance = prevSensorThresholdDistance * 100;
    } else {
      sensorThresholdDistance = 1000;
      EEPROM.put(EEP_ADDR_THRESHOLD_DISTANCE, 10);
    }

    Serial.print(F("THRESHOLD: "));
    Serial.println(sensorThresholdDistance);
  }

  void initEscapeCountdown() {
    int prevEscapeCountdown = 0;
    prevEscapeCountdown = EEPROM.read(EEP_ADDR_ESCAPE_COUNT_DOWN);
    if (prevEscapeCountdown == 5 || prevEscapeCountdown == 10 || prevEscapeCountdown == 15) {
      escapeCountDown = prevEscapeCountdown;
    } else {
      escapeCountDown = 10;
      EEPROM.put(EEP_ADDR_ESCAPE_COUNT_DOWN, 10);
    }

    Serial.print(F("ESCAPE COUNTDOWN: "));
    Serial.println(escapeCountDown);
  }

  void initTriggerDuration() {  // 防抖
    int prevTriggerDuration = 0;
    prevTriggerDuration = EEPROM.read(EEP_ADDR_TRIGGER_DURATION);
    if (prevTriggerDuration == 2 || prevTriggerDuration == 5 || prevTriggerDuration == 10 || prevTriggerDuration == 15 || prevTriggerDuration == 20) {
      triggerDuration = prevTriggerDuration * 100;
    } else {
      triggerDuration = 500;
      EEPROM.put(EEP_ADDR_TRIGGER_DURATION, 5);
    }

    Serial.print(F("TRIGGER DURATION: "));
    Serial.println(triggerDuration);
  }

  void printOneSensor(byte i) {
    uint16_t distance = map(constrain(analogInputs[i].value, 195, 1000), 195, 1000, 0, 1830);
    Serial.print(F("Sensor:"));
    Serial.print(i);

    Serial.print(F(", reading:"));
    Serial.print(analogInputs[i].value);

    Serial.print(F(", distance:"));
    Serial.print(distance);

    if (distance <= sensorThresholdDistance) {
      Serial.print(F(", within threshold "));
      Serial.print(F("["));
      Serial.print(sensorThresholdDistance);
      Serial.println(F("]"));
    } else {
      Serial.println();
    }
  }

  bool canPublish = true;
  void prepareIoTSignal() {
    if (this->status == SYS_RUNNING) {
      DIPayload |= 1 << 4;  // X000 1000
      DIPayload |= 0 << 5;
      DIPayload |= 0 << 6;
      DIPayload |= 0 << 7;
    } else if (this->status == SYS_STOPPED) {
      DIPayload |= 1 << 4;
      DIPayload |= 1 << 5;  // X000 1100
      DIPayload |= 0 << 6;
      DIPayload |= 0 << 7;

      // can publish
      if (canPublish) {
        iot.buildMsg(DIPayload, DOPayload, AIPayload, AOPayload, SWPayload);
        iot.forcePublish();
        alarmTimer.refresh();
        canPublish = false;
      }

    } else if (this->status == SYS_ALLOW_10S) {
      DIPayload |= 1 << 4;
      DIPayload |= 0 << 5;
      DIPayload |= 1 << 6;  // X000 1010
      DIPayload |= 0 << 7;
    } else if (this->status == SYS_FAILURE) {
      DIPayload |= 0 << 4;
      DIPayload |= 0 << 5;
      DIPayload |= 0 << 6;
      DIPayload |= 0 << 7;  // X000 0000
    }
  }

  bool isFailure() {
    bool flag = false;
    for (size_t i = 0; i < sensorNum; i++) {
      if (analogInputs[i].value < 50) {
        flag = true;
      }
    }
    return flag;
  }


public:
  SubSystem(void) {}

  void init() {
    configAnalogInputResolution(0);
    this->status = SYS_RUNNING;
    rStd485.init();
    // this->forceInitEEP();
    this->initThresholdDistance();
    this->initEscapeCountdown();
    this->initTriggerDuration();
  }

  void loop() {
    powerLight.connect();

    // if (this->isFailure()) {
    //   this->status = SYS_FAILURE;
    // } else {
    //   this->status = this->prevStatus;
    // }

    if (this->status == SYS_RUNNING) {
      this->prevStatus = SYS_RUNNING;

      relay.connect();
      alarm.cut();
      warningLight.cut();

      trafficGreen.connect();
      trafficYellow.cut();
      trafficRed.cut();

      if (this->isOneDetected()) {
        if (triggerTimer.autoTimeout(triggerDuration)) {
          this->status = SYS_STOPPED;
          Serial.println(F("SYS_STOPPED"));
        }
      } else {
        triggerTimer.refresh();
      }

    } else if (this->status == SYS_STOPPED) {
      this->prevStatus = SYS_STOPPED;

      relay.cut();
      alarm.connect();
      warningLight.connect();

      trafficGreen.cut();
      trafficYellow.cut();
      trafficRed.connect();

      if (this->areAllEscaped()) {
        this->status = SYS_RUNNING;
      }

      if (pressButton.getState()) {
        this->status = SYS_ALLOW_10S;
        this->tenSecondTimer = millis();
      }

    } else if (this->status == SYS_ALLOW_10S) {
      this->prevStatus = SYS_ALLOW_10S;

      relay.connect();
      alarm.cut();
      warningLight.cut();

      trafficGreen.connect();
      trafficYellow.connect();
      trafficRed.cut();

      if ((millis() - this->tenSecondTimer) >= escapeCountDown * 1000) {
        this->status = SYS_RUNNING;
      }
    }

    if (alarmTimer.autoTimeout(10000)) {
      canPublish = true;
    }

    this->prepareIoTSignal();

    iot.buildMsg(DIPayload, DOPayload, AIPayload, AOPayload, SWPayload);

    rStd485.loop();
  }

  bool isOneDetected() {
    for (size_t i = 0; i < sensorNum; i++) {
      uint16_t distance = map(constrain(analogInputs[i].value, 195, 1000), 195, 1000, 0, 1800);
      uint16_t thresholdDistance = sensorThresholdDistance;
      if (distance <= thresholdDistance) {
        return true;
      }
    }
    return false;
  }

  bool areAllEscaped() {
    bool flag = true;
    for (size_t i = 0; i < sensorNum; i++) {
      uint16_t distance = map(constrain(analogInputs[i].value, 195, 1000), 195, 1000, 0, 1800);
      uint16_t thresholdDistance = sensorThresholdDistance + 25;
      if (distance <= thresholdDistance) {
        flag = false;
      }
    }
    return flag;
  }

  void debug(int i = -1) {
    Serial.println(F("======= DEBUG ======="));
    if (i < 0) {
      for (size_t nth = 0; nth < sensorNum; nth++) {
        this->printOneSensor(nth);
      }
    } else {
      this->printOneSensor(i);
    }
    Serial.println();
  }

  ~SubSystem() {}
};

#endif