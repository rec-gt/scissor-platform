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
  byte id;
  DigitalOutput &di;
  bool isCut = false;
  unsigned long prevMillis = 0;

public:
  Relay(byte id, DigitalOutput &di)
    : id(id) {}

  void cut() {
    this->di.cut();
    this->isCut = true;
    this->prevMillis = millis();
  }

  void debounceConnect() {
    if (this->isCut) {
      if (millis() - this->prevMillis >= 10000UL) {
        this->isCut = false;
        this->di.connect();
      }
    }
  }

  void forceConnect() {
    this->isCut = false;
    this->di.connect();
  }
};

class TempSensor {
private:
  byte id;
  AnalogInputFaster &ai;
  int reading;
  int actualTemp;
  int aoValue;

  int readingToActualTemp(int reading) {
    return map(constrain(reading, 196, 1023), 196, 1023, 0, 1300);
  }

public:
  TempSensor(byte id, AnalogInputFaster &ai)
    : id(id), ai(ai) {}

  void listen() {
    this->reading = this->ai.getValue();
    this->actualTemp = this->readingToActualTemp(this->reading);
    this->aoValue = map(constrain(this->actualTemp, 0, 1300), 0, 1300, 0, 255);

    Serial.print(this->id);
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

Relay relay1(DO_1, digitalOutputs[DO_1]);
Relay relay2(DO_2, digitalOutputs[DO_2]);
Relay relay3(DO_3, digitalOutputs[DO_3]);

TempSensor tempSensor1(AI_1, analogInputs[AI_1]);
TempSensor tempSensor2(AI_2, analogInputs[AI_2]);
TempSensor tempSensor3(AI_3, analogInputs[AI_3]);

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