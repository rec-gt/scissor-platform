#include "detectSystem.h"
#include "pressButton.h"
#include "relay.h"
#include "light.h"
#include "speaker.h"
#include "countdown.h"
#include "laserSensor.h"
#include "baseThresholdSwitch.h"
#include "displayOLED.h"

DetectSystem detectSystem;

DisplayOLED displayOLED;

PressButton pressButton(3);
Relay relay(4);
Light powerLight(6);
Light warningLight(7);
Speaker speaker(10);
BaseThresholdSwitch baseThresholdSwitch(12);  // OK

CountdownTimer countdownTimer(10);

LaserSensor sensors[] = {
  LaserSensor(A0, 0),
  LaserSensor(A1, 0),
  LaserSensor(A2, 0),
  LaserSensor(A3, 0),
  LaserSensor(A4, 0),
  // LaserSensor(A5, 0),
  // LaserSensor(A6, 0),
  // LaserSensor(A7, 0),
  // LaserSensor(A8, 0),
  // LaserSensor(A9, 0),
};

LaserSensorManager sensorsManager(sensors, sizeof(sensors) / sizeof(sensors[0]));


void setup() {
  Serial.begin(9600);

  if (!displayOLED.init()) {
    relay.cut();
  }

  detectSystem.setStatus(RUNNING);

  powerLight.on();
}

void loop() {
  pressButton.listen();

  baseThresholdSwitch.listen();

  sensorsManager.changeBaseThreshold(baseThresholdSwitch.isOn());

  if (detectSystem.getStatus() == RUNNING) {
    displayOLED.print("", "系統運作中", "", 2);
    relay.connect();
    warningLight.off();
    speaker.off();

    if (sensorsManager.isDetected()) {
      detectSystem.setStatus(STOPPED);
    }
  }

  if (detectSystem.getStatus() == STOPPED) {
    displayOLED.print("偵測到障礙物", "系統暫停運作", "", 3);
    relay.cut();
    warningLight.on();
    speaker.on();

    // 1. sensor keep detection, once escape from obstacle, switch to RUNNING
    if (sensorsManager.isEscaped()) {
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
    speaker.off();
    countdownTimer.countdown(displayOLED, countDownCallback);
  }

  delay(1000);
}

void countDownCallback() {
  detectSystem.setStatus(RUNNING);
}