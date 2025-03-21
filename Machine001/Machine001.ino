#include "DetectSystem.h"
#include "PressButton.h"
#include "Relay.h"
#include "Light.h"
#include "Countdown.h"
#include "LaserSensor.h"
#include "BaseThresholdSwitch.h"
#include "DisplayOLED.h"
#include "NBIoT.h"
#include "WarningSystem.h"
#include "TrafficLight.h"
#include "Utils.h"

DetectSystem detectSystem;

DisplayOLED displayOLED;

Utils utils;

PressButton pressButton(12);

BaseThresholdSwitch baseThresholdSwitch(12);

Relay relay(25);

Light TenSecondLight(26);

Light powerLight(27);

WarningSystem warningSystem(28);

TrafficLight trafficLight(31, 30, 29);

CountdownTimer countdownTimer;

NBIoT nbiot;

LaserSensor sensors[] = {
  // (longer threshold, shorter threshold, fine tune)
  LaserSensor(A0, 800, 500, 203),  // 工廠fine-tune，遮擋鏡頭做測試
  LaserSensor(A1, 800, 500, 216),
  LaserSensor(A2, 800, 500, 215),
  LaserSensor(A3, 800, 500, 232),
  LaserSensor(A4, 800, 500, 222),
  LaserSensor(A5, 800, 500, 216),
  LaserSensor(A6, 800, 500, 234),
  LaserSensor(A7, 800, 500, 229),
  LaserSensor(A8, 800, 500, 227),
  LaserSensor(A9, 800, 500, 222),
};

LaserSensorManager sensorsManager(sensors, sizeof(sensors) / sizeof(sensors[0]));

void setup() {
  Serial.begin(9600);

  displayOLED.init();

  detectSystem.set(SYS_RUNNING);

  powerLight.on();
}

void loop() {
  // ========= handling press button =========
  pressButton.listen();


  // ========= handling threshold switch =========
  baseThresholdSwitch.listen();
  sensorsManager.changeAllBaseThreshold(baseThresholdSwitch.on());


  // ========= controlling traffic light =========
  trafficLight.listen(sensorsManager.getMinDistance());


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

  // ========= debugging =========
  // sensorsManager.print(0);
  sensorsManager.printAll();
  // sensorsManager.calibrate();

  delay(1000);
}

void countDownCallback() {
  detectSystem.set(SYS_RUNNING);
}