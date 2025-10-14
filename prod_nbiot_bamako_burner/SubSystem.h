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
  DigitalOutput &doModule;
  bool isCut = false;
  unsigned long prevMillis = 0;

public:
  Relay(byte id, DigitalOutput &doModule)
    : id(id), doModule(doModule) {}

  void cut() {
    this->doModule.cut();
    this->isCut = true;
    this->prevMillis = millis();
  }

  void debounceConnect() {
    if (this->isCut) {
      if (millis() - this->prevMillis >= 10000UL) {
        this->isCut = false;
        this->doModule.connect();
      }
    }
  }

  void forceConnect() {
    this->isCut = false;
    this->doModule.connect();
  }
};

class TempSensor {
private:
  byte id;
  AnalogInputFaster &aiModule;
  int reading;
  int actualTemp;
  int aoValue;

  int readingToActualTemp(int reading) {
    return map(constrain(reading, 196, 1023), 196, 1023, 0, 1300);
  }

public:
  TempSensor(byte id, AnalogInputFaster &aiModule)
    : id(id), aiModule(aiModule) {}

  void listen() {
    this->reading = this->aiModule.getValue();
    this->actualTemp = this->readingToActualTemp(this->reading);
    this->aoValue = map(constrain(this->actualTemp, 0, 1300), 0, 1300, 0, 255);

    Serial.print(this->id);
    Serial.print(" : ");
    Serial.println(this->actualTemp);
  }

  void displayTemp(AnalogOutput &aoModule) {
    aoModule.set(this->aoValue + 2);
  }

  void breakpoint(Relay &relay, int threshold) {
    if (this->actualTemp >= threshold) {
      relay.cut();
    } else {
      relay.debounceConnect();
    }
  }
};

class TempSwitch {

}

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
    tempSensor1.displayTemp(analogOutputs[AO_1]);
    tempSensor1.breakpoint(relay1, 800);

    tempSensor2.listen();
    tempSensor2.displayTemp(analogOutputs[AO_2]);
    tempSensor2.breakpoint(relay2, 450);

    tempSensor3.listen();
    tempSensor3.displayTemp(analogOutputs[AO_3]);
    tempSensor3.breakpoint(relay3, 250);
  }

  ~SubSystem() {}
};

#endif