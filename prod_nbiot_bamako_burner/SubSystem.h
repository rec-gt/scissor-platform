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
    Serial.print(F(" : "));
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

  void displayTemp(AnalogInput &aiModule) {
    aiModule.set(this->aoValue + 2);
  }
};

class TempSwitch {
private:
  byte id;
  DigitalInput &diModule;
  byte prevState = 1;
  int minTemp;
  int maxTemp;

public:
  int temperature = 800;

  TempSwitch(byte id, DigitalInput &diModule, int temperature, int minTemp, int maxTemp)
    : id(id), diModule(diModule), temperature(temperature), minTemp(minTemp), maxTemp(maxTemp) {}

  void listen() {
    byte state = this->diModule.getState();

    if (state != this->prevState) {
      this->prevState = state;
      if (state == 1) {
        if (this->temperature <= this->minTemp || this->temperature >= this->maxTemp) {
          this->temperature = this->minTemp;
        } else {
          this->temperature += 10;
        }
      }
    }
  }

  ~TempSwitch(){};
};

Relay relay1(DO_1, digitalOutputs[DO_1]);
Relay relay2(DO_2, digitalOutputs[DO_2]);
Relay relay3(DO_3, digitalOutputs[DO_3]);

TempSensor tempSensor1(AI_1, analogInputs[AI_1]);
TempSensor tempSensor2(AI_2, analogInputs[AI_2]);
TempSensor tempSensor3(AI_3, analogInputs[AI_3]);

TempSwitch tempSwitch1(DI_1, digitalInputs[DI_1], 800, 800, 800);
TempSwitch tempSwitch2(DI_2, digitalInputs[DI_2], 450, 400, 500);
TempSwitch tempSwitch3(DI_3, digitalInputs[DI_3], 250, 200, 300);

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
    tempSensor1.breakpoint(relay1, tempSwitch1.temperature);

    tempSensor2.listen();
    tempSensor2.displayTemp(analogOutputs[AO_2]);
    tempSensor2.breakpoint(relay2, tempSwitch2.temperature);

    tempSensor3.listen();
    tempSensor3.displayTemp(analogOutputs[AO_3]);
    tempSensor3.breakpoint(relay3, tempSwitch3.temperature);

    tempSwitch1.listen();
    tempSwitch2.listen();
    tempSwitch3.listen();
  }

  ~SubSystem() {}
};

#endif