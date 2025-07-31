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
#include "AsyncTimer.h"
#include "Utils.h"
#include "Globals.h"
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

AsyncTimer sysTimer(500);

PublishState forcePublishState = FORCE_PUBLISH_DISABLED;

void setup() {
  analogReference(DEFAULT);
  Serial.begin(9600);
  NBIOT_SERIAL.begin(9600);

  // === nbiot config ===
  nbiot.debug();

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

  // === Global Variable Init ===
  connStr.reserve(255);
  publishMsgForce.reserve(1024);
  publishMsgContent.reserve(1024);

  // === Watchdog Init ===
  wdt_enable(WDTO_8S);

  // ======
  displayOLED.print("", "系統運作中", "", DISPLAY_SYS_RUNNING);
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

    if (forcePublishState != FORCE_PUBLISH_RUNNING) {
      forcePublishState = FORCE_PUBLISH_RUNNING;
    }

  } else if (detectSystem.is(SYS_STOPPED)) {
    relay.cut();
    warningSystem.on();
    tenSecondsLight.on();
    trafficLight.red();

    // 1. sensor keep detection, once escape from obstacle, switch to RUNNING
    if (sensorManager.areAllEscaped()) {
      detectSystem.set(SYS_RUNNING);
    }

    // 2. press button to get 10s moving time
    if (pressButton.isPressed()) {
      detectSystem.set(SYS_ALLOW_10S);
      countdownTimer.set();
    }

    if (forcePublishState != FORCE_PUBLISH_STOPPED) {
      if (sysTimer.autoExpired(1000)) {
        forcePublishState = FORCE_PUBLISH_STOPPED;
        nbiot.forcePublish();
      }
    }

    sensorManager.showOneDetected();

  } else if (detectSystem.is(SYS_ALLOW_10S)) {
    relay.connect();
    warningSystem.off();
    tenSecondsLight.off();
    trafficLight.greenYellow();

    countdownTimer.countdown([]() {
      detectSystem.set(SYS_RUNNING);
    });

  } else if (detectSystem.is(SYS_FAILURE)) {
    relay.cut();
    trafficLight.redYellow();

    sensorManager.showOneUnhealthy();

    if (sensorManager.areAllHealthy()) {
      detectSystem.set(SYS_RUNNING);
    }
  }

  // === pet the watchdog ===
  wdt_reset();

  delay(10);
}