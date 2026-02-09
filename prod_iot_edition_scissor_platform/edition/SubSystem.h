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

class SubSystem {
private:
  SubSystemStatus status;

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

  void initThresholdDistance() {
    int prevSensorThresholdDistance = 0;
    prevSensorThresholdDistance = EEPROM.read(EEP_ADDR_THRESHOLD_DISTANCE);
    if (prevSensorThresholdDistance == 6 || prevSensorThresholdDistance == 8 || prevSensorThresholdDistance == 10 || prevSensorThresholdDistance == 12) {
      sensorThresholdDistance = prevSensorThresholdDistance * 100;
    } else {
      sensorThresholdDistance = 1000;
      EEPROM.put(EEP_ADDR_THRESHOLD_DISTANCE, 5);
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
      escapeCountDown = 5;
      EEPROM.put(EEP_ADDR_ESCAPE_COUNT_DOWN, 5);
    }

    Serial.print(F("ESCAPE COUNTDOWN: "));
    Serial.println(escapeCountDown);
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

  void prepareAlarmSignal() {
    SWPayload = 0;
    if (this->status == SYS_RUNNING) {
      SWPayload = 1;  // 0001
    } else if (this->status == SYS_STOPPED) {
      SWPayload = 3;  // 0011, RUNNING, but obstacle detected
      // iot.forcePublish();
    } else if (this->status == SYS_ALLOW_10S) {
      SWPayload = 5;  // 0101, RUNNING, but escaping
      // iot.forcePublish();
    }
  }

public:
  SubSystem(void) {}

  void init() {
    configAnalogInputResolution(0);
    this->status = SYS_RUNNING;
    rStd485.init();
    this->initThresholdDistance();
    this->initEscapeCountdown();
  }

  void loop() {
    this->prepareAlarmSignal();

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

      if ((millis() - this->tenSecondTimer) >= escapeCountDown * 1000) {
        this->status = SYS_RUNNING;
      }
    }

    rStd485.loop();
  }

  bool isOneDetected() {
    for (size_t i = 0; i < 10; i++) {
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
    for (size_t i = 0; i < 10; i++) {
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
      for (size_t nth = 0; nth < 10; nth++) {
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