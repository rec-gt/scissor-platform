#include "DetectSystem.h"
#include "PressButton.h"
#include "Relay.h"
#include "Light.h"
#include "Countdown.h"
#include "LaserSensor.h"
#include "SensorManager.h"
#include "BaseThresholdSwitch.h"
#include "DisplayOLED.h"
#include "NBIoT.h"
#include "WarningSystem.h"
#include "TrafficLight.h"
#include "Utils.h"
#include <avr/wdt.h>

DetectSystem detectSystem;

DisplayOLED displayOLED;

Utils utils;

BaseThresholdSwitch baseThresholdSwitch(4);

PressButton pressButton(5);

WarningSystem warningSystem(24);

Light tenSecondsLight(25);

Light powerLight(26);

TrafficLight trafficLight(30, 29, 28);

Relay relay(31);

CountdownTimer countdownTimer;

NBIoT nbiot;

LaserSensor sensors[] = {
  LaserSensor(A0, 210),
  LaserSensor(A1, 210),
  LaserSensor(A2, 210),
  LaserSensor(A3, 210),
  LaserSensor(A4, 210),
  LaserSensor(A5, 210),
  LaserSensor(A6, 210),
  LaserSensor(A7, 210),
  LaserSensor(A8, 210),
  LaserSensor(A9, 210),
};

LaserSensorManager sensorManager(sensors, sizeof(sensors) / sizeof(sensors[0]));

void setup() {
  analogReference(DEFAULT);
  Serial.begin(9600);

  // === System Starting ===
  relay.cut();
  tenSecondsLight.off();
  trafficLight.off();
  warningSystem.off();
  displayOLED.init();
  nbiot.init();
  powerLight.on();
  detectSystem.set(SYS_RUNNING);

  delay(500);

  // === watchdog ===
  wdt_enable(WDTO_8S);
}

void loop() {
  // === handling sensors ===
  sensorManager.listenAll();

  // === handling press button ===
  pressButton.listen();

  // === handling threshold switch ===
  baseThresholdSwitch.listen();
  sensorManager.setAllBaseThreshold(baseThresholdSwitch.on());

  // === handling detection system ===
  if (detectSystem.is(SYS_RUNNING)) {
    relay.connect();
    warningSystem.off();
    tenSecondsLight.off();
    trafficLight.listen(sensorManager.getMinDistance());

    detectSystem.publishStatus(3);

    if (sensorManager.isOneDetected()) {
      detectSystem.set(SYS_STOPPED);
    }

    if (!sensorManager.areAllHealthy()) {
      detectSystem.set(SYS_FAILURE);
    }

  } else if (detectSystem.is(SYS_STOPPED)) {
    relay.cut();
    warningSystem.on();
    tenSecondsLight.on();
    trafficLight.red();

    detectSystem.publishStatus(1);

    if (sensorManager.areAllEscaped()) {  // 1. sensor keep detection, once escape from obstacle, switch to RUNNING
      detectSystem.set(SYS_RUNNING);
    }

    if (pressButton.isPressed()) {  // 2. press button to get 10s moving time
      detectSystem.set(SYS_ALLOW_10S);
      countdownTimer.set();
    }

  } else if (detectSystem.is(SYS_ALLOW_10S)) {
    relay.connect();
    warningSystem.off();
    tenSecondsLight.off();
    countdownTimer.countdown(countDownCallback);

    detectSystem.publishStatus(2);

  } else if (detectSystem.is(SYS_FAILURE)) {
    relay.cut();
    warningSystem.on();

    if (sensorManager.areAllHealthy()) {
      detectSystem.set(SYS_RUNNING);
    }
  }

  // === send MQTT ===
  detectSystem.publishStatus();

  // === pet the dog ===
  wdt_reset();

  // === Debugging ===
  // sensorManager.printOne(0);
  // sensorManager.printAll();

  delay(20);
}

void countDownCallback() {
  detectSystem.set(SYS_RUNNING);
}