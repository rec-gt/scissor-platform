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
#include "RS485Module.h"

DetectSystem detectSystem;

DisplayOLED displayOLED;

Utils utils;

PressButton pressButton(5);

BaseThresholdSwitch baseThresholdSwitch(6);

Relay relay(24);

Light tenSecondLight(25);

Light powerLight(26);

WarningSystem warningSystem(28);

TrafficLight trafficLight(31, 30, 29);

CountdownTimer countdownTimer;

NBIoT nbiot;

LaserSensor sensors[] = {
  // (pin, longer threshold, shorter threshold, fine tune)
  LaserSensor(A0, 800, 500, 220),  // 工廠fine-tune，遮擋鏡頭做測試
  LaserSensor(A1, 800, 500, 282),
  LaserSensor(A2, 800, 500, 330),
  LaserSensor(A3, 800, 500, 330),
  LaserSensor(A4, 800, 500, 330),
  LaserSensor(A5, 800, 500, 330),
  LaserSensor(A6, 800, 500, 330),
  LaserSensor(A7, 800, 500, 330),
  LaserSensor(A8, 800, 500, 330),
  LaserSensor(A9, 800, 500, 330),
};

LaserSensorManager sensorsManager(sensors, sizeof(sensors) / sizeof(sensors[0]));

// DownwardSensor downwardSensor(A11, 200);

RS485Module rs485;


void setup() {
  Serial.begin(9600);

  powerLight.off();
  tenSecondLight.off();
  trafficLight.off();
  warningSystem.off();
  relay.cut();

  displayOLED.init();

  nbiot.init();

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
    tenSecondLight.off();

    if (sensorsManager.isOneDetected()) {
      detectSystem.set(SYS_STOPPED);
    }

    if (!sensorsManager.areAllHealthy()) {
      detectSystem.set(SYS_FAILURE);
    }
  } else if (detectSystem.is(SYS_STOPPED)) {
    relay.cut();
    warningSystem.on();
    tenSecondLight.on();

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
    tenSecondLight.off();
    countdownTimer.countdown(countDownCallback);
  } else if (detectSystem.is(SYS_FAILURE)) {
    relay.cut();
    warningSystem.on();
    if (sensorsManager.areAllHealthy()) {
      detectSystem.set(SYS_RUNNING);
    }
  }

  // ========= NB-IoT =========
  // nbiot.publish(sensorsManager.getSensors8Status(), sensorsManager.getSensors2Status(), detectSystem.getStatus(), 0);

  // ========= Debugging =========
  // sensorsManager.printOne(0);
  Serial.println();
  sensorsManager.printAll();

  // rs485.waitForMsg();

  delay(1000);
}

void countDownCallback() {
  detectSystem.set(SYS_RUNNING);
}