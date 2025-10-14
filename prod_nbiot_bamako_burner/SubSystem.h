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

class TempSensor {
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
  TempSensor(byte nth)
    : nth(nth) {}

  void listen() {
    this->reading = analogInputs[this->nth].getValue();
    this->actualTemp = this->readingToActualTemp(this->reading);
    this->aoValue = map(this->actualTemp, 0, 1300, 0, 255);

    Serial.print(this->nth);
    Serial.print(" : ");
    Serial.println(this->actualTemp);
  }

  void displayTemp(byte targetNth) {
    analogOutputs[targetNth].set(this->aoValue + 2);
  }

  void breakpoint(Relay &relay, int threshold) {
    if (this->actualTemp >= threshold) {
      relay.cut();
    } else {
      relay.debounceConnect();
    }
  }
};

Relay relay1(DI_1);
Relay relay2(DI_2);
Relay relay3(DI_3);

TempSensor tempSensor1(AI_1);
TempSensor tempSensor2(AI_2);
TempSensor tempSensor3(AI_3);

class SubSystem {
public:
  SubSystem(void) {
    relay1.forceConnect();
    relay2.forceConnect();
    relay3.forceConnect();
  }

  void loop() {
    tempSensor1.listen();
    tempSensor1.displayTemp(AO_1);
    tempSensor1.breakpoint(relay1, 800);

    tempSensor2.listen();
    tempSensor2.displayTemp(AO_2);
    tempSensor2.breakpoint(relay2, 450);

    tempSensor3.listen();
    tempSensor3.displayTemp(AO_3);
    tempSensor3.breakpoint(relay3, 250);
  }

  ~SubSystem() {}
};

#endif