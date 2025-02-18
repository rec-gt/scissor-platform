#include "detectSystem.h"
// #include "pressButton.h"
#include "pressButton2Pin.h"
#include "relay.h"
#include "light.h"
#include "warningLight.h"
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
WarningLight warningLight(9);
Speaker speaker(10);
BaseThresholdSwitch baseThresholdSwitch(12);  // OK

CountdownTimer countdownTimer(10);

LaserSensor laserSensors[] = {
  LaserSensor(A0, 0),
  // LaserSensor(A1, 0),
  // LaserSensor(A2, 0),
  // LaserSensor(A3, 0),
  // LaserSensor(A4, 0),
  // LaserSensor(A5, 0),
  // LaserSensor(A6, 0),
  // LaserSensor(A7, 0),
  // LaserSensor(A8, 0),
  // LaserSensor(A9, 0),
};

void setup() {
  Serial.begin(9600);
  if (!displayOLED.init()) {
    relay.cut();
  }
  detectSystem.setStatus(RUNNING);
}

void loop() {
  powerLight.on();

  pressButton.debounceListen();

  baseThresholdSwitch.listen();
  setBaseThreshold(baseThresholdSwitch.isOn());

  if (detectSystem.getStatus() == RUNNING) {
    relay.connect();
    warningLight.off();
    speaker.off();
    listenSensors();
  }

  if (detectSystem.getStatus() == STOPPED) {
    relay.cut();
    warningLight.on();
    speaker.on();

    // sensor keep detection, once escape from obstacle, switch to RUNNING
    dangerListenSensors();

    if (pressButton.isPressed()) {
      // Serial.println("10s Button Pressed");
      // detectSystem.setStatus(ALLOW_10S);
      // countdownTimer.setStart(millis());
    }
  }

  if (detectSystem.getStatus() == ALLOW_10S) {
    relay.connect();
    warningLight.off();
    speaker.off();
    countdownTimer.countdown(countDownCallback);
  }

  delay(100);
}

void setBaseThreshold(bool toggle) {
  int numLaserSensors = sizeof(laserSensors) / sizeof(laserSensors[0]);
  for (int i = 0; i < numLaserSensors; i++) {
    laserSensors[i].setBaseThreshold(toggle);  // true = 300, false = 500
  }
}

void listenSensors() {
  int numLaserSensors = sizeof(laserSensors) / sizeof(laserSensors[0]);
  for (int i = 0; i < numLaserSensors; i++) {
    laserSensors[i].debounceListen();
    laserSensors[i].print().byValue();

    if (laserSensors[i].isDetected()) {
      Serial.println("Obstacle Detected!");
      detectSystem.setStatus(STOPPED);
      break;
    };
  }
}

void dangerListenSensors() {
  int numLaserSensors = sizeof(laserSensors) / sizeof(laserSensors[0]);
  for (int i = 0; i < numLaserSensors; i++) {
    laserSensors[i].setDangerBuffer(true);
    laserSensors[i].debounceListen();
    laserSensors[i].print().byValue();

    if (!laserSensors[i].isDetected()) {
      Serial.println("Vehicle Escaped from Obstacle!");
      detectSystem.setStatus(RUNNING);
      break;
    };
  }
}

void countDownCallback() {
  Serial.println("Countdown Finish!");
  detectSystem.setStatus(RUNNING);
}