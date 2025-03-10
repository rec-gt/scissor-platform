#include "DetectSystem.h"
#include "PressButton.h"
#include "Relay.h"
#include "Light.h"
#include "Countdown.h"
#include "LaserSensor.h"
#include "DownwardSensor.h"
#include "BaseThresholdSwitch.h"
#include "DisplayOLED.h"
#include "NBIoT.h"
#include "WarningSystem.h"
#include "TrafficLight.h"
#include "Utils.h"


DetectSystem detectSystem;

DisplayOLED displayOLED;

Utils utils;

Relay relay(30);

PressButton pressButton(28);

BaseThresholdSwitch baseThresholdSwitch(26);

Light powerLight(22);

WarningSystem warningSystem(24);

TrafficLight trafficLight(14, 15, 16);

CountdownTimer countdownTimer;

NBIoT nbIot;

LaserSensor sensors[] = {
  LaserSensor(A0, 203),  // 現場或工廠fine-tune，遮擋鏡頭做測試
  LaserSensor(A1, 216),
  LaserSensor(A2, 215),
  LaserSensor(A3, 232),
  LaserSensor(A4, 222),
  LaserSensor(A5, 216),
  LaserSensor(A6, 234),
  LaserSensor(A7, 229),
  LaserSensor(A8, 227),
  LaserSensor(A9, 222),
};

DownwardSensor downwardSensor = DownwardSensor(A10, 216);

LaserSensorManager sensorsManager(sensors, sizeof(sensors) / sizeof(sensors[0]));

void setup() {
  Serial.begin(9600);

  powerLight.off();
  trafficLight.off();
  warningSystem.off();
  relay.cut();

  displayOLED.init();

  nbIot.init();

  detectSystem.set(SYS_RUNNING);

  powerLight.on();
}

void loop() {
  // ========= handling press button =========
  pressButton.listen();


  // ========= handling threshold switch =========
  baseThresholdSwitch.listen();
  sensorsManager.changeBaseThreshold(baseThresholdSwitch.on());


  // ========= controlling traffic light =========
  downwardSensor.listen();

  if (downwardSensor.isUp()) {
    trafficLight.listen(sensorsManager.getMinDistance());
  } else {
    trafficLight.off();
  }

  // ========= controlling detection system =========
  if (detectSystem.is(SYS_RUNNING)) {
    relay.connect();
    warningSystem.off();

    if (sensorsManager.isOneDetected()) {
      detectSystem.set(SYS_STOPPED);
    }
    if (!sensorsManager.areAllHealthy()) {
      detectSystem.set(SYS_FAILURE);
    }
  } else if (detectSystem.is(SYS_STOPPED)) {
    relay.cut();
    warningSystem.on();

    if (sensorsManager.areAllEscaped()) {  // 1. sensor keep detection, once escape from obstacle, switch to RUNNING
      detectSystem.set(SYS_RUNNING);
    }

    if (pressButton.isPressed()) {  // 2. press button to get 10s moving time
      detectSystem.set(SYS_ALLOW_10S);
      countdownTimer.set();
    }
  } else if (detectSystem.is(SYS_ALLOW_10S)) {
    relay.connect();
    warningSystem.off();
    countdownTimer.countdown(countDownCallback);
  } else if (detectSystem.is(SYS_FAILURE)) {
    relay.cut();
    warningSystem.on();
    if (sensorsManager.areAllHealthy()) {
      detectSystem.set(SYS_RUNNING);
    }
  }

  // ========= NB-IoT =========
  nbIot.publish(sensorsManager.getSensors8Status(), sensorsManager.getSensors2Status(), detectSystem.getStatus(), downwardSensor.isUp());

  // ========= debugging =========
  // sensorsManager.printAll();
  // sensorsManager.calibrate();
  // downwardSensor.calibrateReading();
  // downwardSensor.calibrateDistance();

  delay(10);
}

void countDownCallback() {
  detectSystem.set(SYS_RUNNING);
}