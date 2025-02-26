#include "DetectSystem.h"
#include "PressButton.h"
#include "Relay.h"
#include "Light.h"
#include "Countdown.h"
#include "LaserSensor.h"
#include "BaseThresholdSwitch.h"
#include "DisplayOLED.h"
#include "WarningSystem.h"
#include "TrafficLight.h"

DetectSystem detectSystem;

DisplayOLED displayOLED;

Relay relay(30);
PressButton pressButton(28);
BaseThresholdSwitch baseThresholdSwitch(26);

Light powerLight(22);
WarningSystem warningSystem(24);
TrafficLight trafficLight(14, 16, 18);

CountdownTimer countdownTimer;

LaserSensor sensors[] = {
  LaserSensor(A0, 213),  //遮擋鏡頭測試
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

LaserSensorManager sensorsManager(sensors, sizeof(sensors) / sizeof(sensors[0]));


void setup() {
  Serial.begin(9600);

  warningSystem.off();
  relay.cut();

  displayOLED.init();

  detectSystem.setStatus(SYS_RUNNING);

  powerLight.on();
}

void loop() {
  pressButton.listen();

  baseThresholdSwitch.listen();

  sensorsManager.changeBaseThreshold(baseThresholdSwitch.is800());

  trafficLight.listen(sensorsManager.getMinDistance());

  if (detectSystem.getStatus() == SYS_RUNNING) {
    relay.connect();
    warningSystem.off();

    if (sensorsManager.isOneDetected()) {
      detectSystem.setStatus(SYS_STOPPED);
    }
    if (!sensorsManager.areAllHealthy()) {
      detectSystem.setStatus(SYS_FAILURE);
    }
  } else if (detectSystem.getStatus() == SYS_STOPPED) {
    relay.cut();
    warningSystem.on();

    // 1. sensor keep detection, once escape from obstacle, switch to RUNNING
    if (sensorsManager.areAllEscaped()) {
      detectSystem.setStatus(SYS_RUNNING);
    }

    // 2. press button to get 10s moving time
    if (pressButton.isPressed()) {
      detectSystem.setStatus(SYS_ALLOW_10S);
      countdownTimer.set();
    }
  } else if (detectSystem.getStatus() == SYS_ALLOW_10S) {
    relay.connect();
    warningSystem.off();
    countdownTimer.countdown(countDownCallback);
  } else if (detectSystem.getStatus() == SYS_FAILURE) {
    relay.cut();
    warningSystem.on();
    if (sensorsManager.areAllHealthy()) {
      detectSystem.setStatus(SYS_RUNNING);
    }
  }

  delay(10);
}

void countDownCallback() {
  detectSystem.setStatus(SYS_RUNNING);
}