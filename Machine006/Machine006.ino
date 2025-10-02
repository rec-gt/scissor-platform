#include "DetectSystem.h"
#include "PressButton.h"
#include "Relay.h"
#include "Light.h"
#include "Countdown.h"
#include "LaserSensor.h"
#include "SensorManager.h"
#include "BaseThresholdSwitch.h"
#include "NBIoT.h"
#include "WarningSystem.h"
#include "TrafficLight.h"
#include "AsyncTimer.h"
#include "Utils.h"
#include "Globals.h"
#include <avr/wdt.h>

DetectSystem detectSystem;

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

void setup() {
  analogReference(DEFAULT);
  Serial.begin(9600);
  NBIoTSerial.begin(9600);
  DisplaySerial.begin(9600);

  /*=== String Management ===*/
  nbiotCSQ.reserve(8);
  nbiotCGATT.reserve(8);
  nbiotCEREG.reserve(8);
  nbiotPubAck.reserve(8);
  nbiotSubAck.reserve(8);
  nbiotSubMsgContent.reserve(8);
  nbiotIMEI.reserve(32);
  cmpStr.reserve(32);
  AOPayload.reserve(64);
  nbIotConnCmd.reserve(64);
  nbiotSubsCmd.reserve(64);
  nbiotPubMsgPrepare.reserve(64);
  AIPayload.reserve(128);
  rs485SerialRecv.reserve(128);
  nbiotSerialRecv.reserve(128);
  nbiotPubMsgPayload.reserve(256);
  nbiotPubMsgCommand.reserve(256);
  bool res = rubbishStr.reserve(278);  //push it to limit

  Serial.print(res ? F("[Str Space OK]") : F("[String Space NOT OK]"));

  // === System Starting ===
  relay.cut();
  tenSecondsLight.off();
  trafficLight.off();
  warningSystem.off();
  // nbiot.debug();
  // nbiot.init(true);
  powerLight.on();
  detectSystem.set(SYS_RUNNING);
  delay(500);

  // === Watchdog Init ===
  wdt_enable(WDTO_8S);
}

void loop() {
  // === Debugging ===
  // sensorManager.printOne(0);
  // sensorManager.printAll(); // 注意，開啓後會帶來延遲

  // === handle NBIoT===
  // nbiot.loop();

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

    // 1. sensor keep detection, once escape from obstacle, switch to RUNNING
    if (sensorManager.areAllEscaped()) {
      detectSystem.set(SYS_RUNNING);
    }

    // 2. press button to get 10s moving time
    if (pressButton.isPressed()) {
      detectSystem.set(SYS_ALLOW_10S);
      countdownTimer.set();
    }

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

    if (sensorManager.areAllHealthy()) {
      detectSystem.set(SYS_RUNNING);
    }
  }

  // === pet the watchdog ===
  wdt_reset();

  detectSystem.handleDisplayContent();

  delay(10);
}