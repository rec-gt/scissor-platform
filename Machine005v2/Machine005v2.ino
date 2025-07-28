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
  LaserSensor(A0),
  LaserSensor(A1),
  LaserSensor(A2),
  LaserSensor(A3),
  LaserSensor(A4),
  LaserSensor(A5),
  LaserSensor(A6),
  LaserSensor(A7),
  LaserSensor(A8),
  LaserSensor(A9),
};

LaserSensorManager sensorManager(sensors, sizeof(sensors) / sizeof(sensors[0]));

void setup() {
  analogReference(DEFAULT);
  Serial.begin(9600);
  NBIOT_SERIAL.begin(9600);

  // === System Starting ===
  relay.cut();
  tenSecondsLight.off();
  trafficLight.off();
  warningSystem.off();
  displayOLED.init();
  nbiot.init();
  displayOLED.print("", "IoT CSQ", nbiot.CSQ.c_str(), DISPLAY_IOT_CSQ);
  delay(3000);
  displayOLED.print("", "IoT CGATT", nbiot.CGATT.c_str(), DISPLAY_IOT_CGATT);
  delay(3000);
  powerLight.on();
  detectSystem.set(SYS_RUNNING);
  delay(500);
  // === watchdog ===
}

void loop() {
  // === Debugging ===
  // sensorManager.printOne(0);
  // sensorManager.printAll(); // 注意，開啓後會帶來延遲

  // === handle NBIoT===
  nbiot.loop();

  // === handling press button ===
  pressButton.listen();

  // === handling threshold switch ===
  baseThresholdSwitch.listen();
  sensorManager.setAllBaseThreshold(baseThresholdSwitch.on());

  // === handling sensors ===
  sensorManager.listenAll();

  // === handling publish message ===
  detectSystem.setPublishMsg();

  // === handling detection system ===
  if (detectSystem.is(SYS_RUNNING)) {
    relay.connect();
    warningSystem.off();
    tenSecondsLight.off();
    trafficLight.listen(sensorManager.getMinDistance());


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
    countdownTimer.countdown([]() {
      detectSystem.set(SYS_RUNNING);
    });

  } else if (detectSystem.is(SYS_FAILURE)) {
    relay.cut();
    warningSystem.on();

    if (sensorManager.areAllHealthy()) {
      detectSystem.set(SYS_RUNNING);
    }
  }

  // === pet the dog ===

  delay(100);
}