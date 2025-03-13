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

Relay relay(30);

PressButton pressButton(28);

BaseThresholdSwitch baseThresholdSwitch(26);

Light powerLight(22);

WarningSystem warningSystem(24);

TrafficLight trafficLight(14, 15, 16);

CountdownTimer countdownTimer;

NBIoT nbiot;

LaserSensor sensors[] = {
  // (longer threshold, shorter threshold, fine tune)
  LaserSensor(A0, 1200, 500, 203),  // 工廠fine-tune，遮擋鏡頭做測試
  LaserSensor(A1, 1200, 500, 216),
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

  powerLight.off();
  trafficLight.off();
  warningSystem.off();
  relay.cut();

  displayOLED.init();

  // nbiot.init();

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

  // ========= NB-IoT =========
  nbiot.publish(sensorsManager.getSensors8Status(), sensorsManager.getSensors2Status(), detectSystem.getStatus(), 0);

  // ========= debugging =========
  sensorsManager.print(0);
  // sensorsManager.printAll();
  // sensorsManager.calibrate();

  delay(1000);
}

void countDownCallback() {
  detectSystem.set(SYS_RUNNING);
}