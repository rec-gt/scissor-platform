#include "DigitalInput.h"
#include "DigitalOutput.h"
#include "AnalogInput.h"
#include "AnalogOutput.h"
#include "NBIoT.h"
#include "DisplayClient.h"
#include "Utils.h"
#include "Globals.h"

#ifndef SubSystem_H
#define SubSystem_H

class Relay {
private:
  byte nth;
  bool isCut = false;
  unsigned long prevMillis = 0;

public:
  Relay(byte nth)
    : nth(nth) {}

  void cut() {
    digitalOutputs[this->nth].cut();
    this->isCut = true;
    this->prevMillis = millis();
  }

  void debounceConnect() {
    if (this->isCut) {
      if (millis() - this->prevMillis >= 10000UL) {
        this->isCut = false;
        digitalOutputs[this->nth].connect();
      }
    }
  }

  void forceConnect() {
    this->isCut = false;
    digitalOutputs[this->nth].connect();
  }
};

class TemperatureSensor {
private:
  byte nth;
  int reading;
  int actualTemp;
  int aoValue;

  int readingToActualTemp(int reading) {
    if (reading < 196) {
      return 0;
    } else {
      return map(reading, 196, 1023, 0, 1300);
    }
  }

public:
  TemperatureSensor(byte nth)
    : nth(nth) {}

  void listen() {
    this->reading = analogInputs[this->nth].getValue();
    this->actualTemp = this->readingToActualTemp(this->reading);
    this->aoValue = map(this->actualTemp, 0, 1300, 0, 255);

    Serial.println(this->reading);
    Serial.println(this->actualTemp);
    Serial.println(this->aoValue);
  }

  void displayTemp(byte targetNth) {
    analogOutputs[targetNth].set(this->aoValue + 1);
  }

  void breakpoint(Relay &relay, int threshold) {
    if (this->actualTemp >= threshold) {
      relay.cut();
    } else {
      relay.debounceConnect();
    }
  }
};

Relay relay1(0);
Relay relay2(1);
Relay relay3(2);

TemperatureSensor temperatureSensor1(0);
TemperatureSensor temperatureSensor2(1);
TemperatureSensor temperatureSensor3(2);

class SubSystem {
public:
  SubSystem(void) {
    relay1.forceConnect();
    relay2.forceConnect();
    relay3.forceConnect();
  }

  void loop() {
    temperatureSensor1.listen();
    temperatureSensor1.displayTemp(0);
    temperatureSensor1.breakpoint(relay1, 60);

    temperatureSensor2.listen();
    temperatureSensor2.displayTemp(1);
    temperatureSensor2.breakpoint(relay2, 60);

    temperatureSensor3.listen();
    temperatureSensor3.displayTemp(2);
    temperatureSensor3.breakpoint(relay3, 60);
  }

  ~SubSystem() {}
};

#endif