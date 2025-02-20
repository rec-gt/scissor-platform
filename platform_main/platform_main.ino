#include "detectSystem.h"
#include "pressButton.h"
#include "relay.h"
#include "light.h"
#include "countdown.h"
#include "laserSensor.h"
#include "baseThresholdSwitch.h"
#include "displayOLED.h"

DetectSystem detectSystem;

DisplayOLED displayOLED;

Relay relay(30);
PressButton pressButton(28);
BaseThresholdSwitch baseThresholdSwitch(26);
Light powerLight(24);
Light warningLight(22);

CountdownTimer countdownTimer(10);

LaserSensor sensors[] = {
  LaserSensor(A0, 0),
  LaserSensor(A1, 0),
  LaserSensor(A2, 0),
  LaserSensor(A3, 0),
  LaserSensor(A4, 0),
  LaserSensor(A5, 0),
  LaserSensor(A6, 0),
  LaserSensor(A7, 0),
  LaserSensor(A8, 0),
  LaserSensor(A9, 0),
};

LaserSensorManager sensorsManager(sensors, sizeof(sensors) / sizeof(sensors[0]));


void setup() {
  Serial.begin(9600);

  warningLight.off();
  relay.cut();

  if (!displayOLED.init()) {
    relay.cut();
  }

  detectSystem.setStatus(RUNNING);

  powerLight.on();
}

void loop() {
  pressButton.listen();

  baseThresholdSwitch.listen();

  sensorsManager.changeBaseThreshold(baseThresholdSwitch.is800());

  if (detectSystem.getStatus() == RUNNING) {
    displayOLED.print("", "系統運作中", "", 2);
    relay.connect();
    warningLight.off();

    if (sensorsManager.isOneDetected()) {
      detectSystem.setStatus(STOPPED);
    }
  }

  if (detectSystem.getStatus() == STOPPED) {
    displayOLED.print("偵測到障礙物", "系統暫停運作", "", 3);
    relay.cut();
    warningLight.on();

    // 1. sensor keep detection, once escape from obstacle, switch to RUNNING
    if (sensorsManager.isAllEscaped()) {
      detectSystem.setStatus(RUNNING);
    }

    // 2. press button to get 10s moving time
    if (pressButton.isPressed()) {
      detectSystem.setStatus(ALLOW_10S);
      countdownTimer.set();
    }
  }

  if (detectSystem.getStatus() == ALLOW_10S) {
    relay.connect();
    warningLight.off();
    countdownTimer.countdown(displayOLED, countDownCallback);
  }

  delay(60);
}

void countDownCallback() {
  detectSystem.setStatus(RUNNING);
}