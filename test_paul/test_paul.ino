#include "Relay.h"
#include "AnalogInput.h"
#include <avr/wdt.h>

Relay relay(24);

AnalogInput ai1 = analogInput(A1);
AnalogInput ai2 = analogInput(A2);
AnalogInput ai3 = analogInput(A4);
AnalogInput ai4 = analogInput(A4);

void setup() {
  analogReference(EXTERNAL);

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

  // === handling traffic light ===
  trafficLight.listen(sensorManager.getMinDistance());

  // === handling detection system ===
  if (detectSystem.is(SYS_RUNNING)) {
    detectSystem.publishStatus(3);

    relay.connect();
    warningSystem.off();
    tenSecondsLight.off();

    if (sensorManager.isOneDetected()) {
      detectSystem.set(SYS_STOPPED);
    }

    if (!sensorManager.areAllHealthy()) {
      detectSystem.set(SYS_FAILURE);
    }

  } else if (detectSystem.is(SYS_STOPPED)) {
    detectSystem.publishStatus(1);

    relay.cut();
    warningSystem.on();
    tenSecondsLight.on();

    if (sensorManager.areAllEscaped()) {  // 1. sensor keep detection, once escape from obstacle, switch to RUNNING
      detectSystem.set(SYS_RUNNING);
    }

    if (pressButton.isPressed()) {  // 2. press button to get 10s moving time
      detectSystem.set(SYS_ALLOW_10S);
      countdownTimer.set();
    }

  } else if (detectSystem.is(SYS_ALLOW_10S)) {
    detectSystem.publishStatus(2);

    relay.connect();
    warningSystem.off();
    tenSecondsLight.off();

    countdownTimer.countdown(countDownCallback);

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

  delay(10);
}

void countDownCallback() {
  detectSystem.set(SYS_RUNNING);
}