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

int readingToActualTemp(int reading) {
  return map(constrain(reading, 196, 1023), 196, 1023, 0, 1300);
}

int getAoValue(int actualTemp) {
  return constrain(map(constrain(actualTemp, 0, 1300), 0, 1300, 0, 255) + (actualTemp * 2 / 100) + 1, 0, 255);
}

// class Relay {
// private:
//   byte id;
//   DigitalOutput &doModule;
//   bool isCut = false;
//   unsigned long prevMillis = 0;

// public:
//   Relay(byte id, DigitalOutput &doModule)
//     : id(id), doModule(doModule) {}

//   void cut() {
//     this->doModule.cut();
//     this->isCut = true;
//     this->prevMillis = millis();
//   }

//   void debounceConnect() {
//     if (this->isCut) {
//       if (millis() - this->prevMillis >= 10000UL) {
//         this->isCut = false;
//         this->doModule.connect();
//       }
//     }
//   }

//   void forceConnect() {
//     this->isCut = false;
//     this->doModule.connect();
//   }
// };

// class TempSensor {
// private:
//   byte id;
//   AnalogInput1024 &aiModule;
//   int reading;
//   int actualTemp;
//   int aoValue;

//   int readingToActualTemp(int reading) {
//     return map(constrain(reading, 196, 1023), 196, 1023, 0, 1300);
//   }

//   int getAoValue(int actualTemp) {
//     return constrain(map(constrain(actualTemp, 0, 1300), 0, 1300, 0, 255) + (actualTemp * 2 / 100) + 1, 0, 255);
//   }

// public:
//   TempSensor(byte id, AnalogInput1024 &aiModule)
//     : id(id), aiModule(aiModule) {}

//   void listen() {
//     this->reading = this->aiModule.getValue();
//     this->actualTemp = this->readingToActualTemp(this->reading);
//     this->aoValue = constrain(map(constrain(this->actualTemp, 0, 1300), 0, 1300, 0, 255) + (this->actualTemp * 2 / 100) + 1, 0, 255);
//     // Serial.print(this->id);
//     // Serial.print(F(" : "));
//     // Serial.println(this->actualTemp);
//   }

//   void displayScreenTemp(AnalogInput &aiDisplayModule) {
//     aiDisplayModule.value = this->actualTemp;
//   }

//   void displayTemp(AnalogOutput &aoModule) {
//     aoModule.set(this->aoValue);
//   }

//   void breakpoint(Relay &relay, int threshold) {
//     if (this->actualTemp >= threshold) {
//       relay.cut();
//     } else {
//       relay.debounceConnect();
//     }
//   }
// };

// class TempSwitch {
// private:
//   byte id;
//   DigitalInput &diModule;
//   byte prevState = 1;
//   int minTemp;
//   int maxTemp;

// public:
//   int temperature = 800;

//   TempSwitch(byte id, DigitalInput &diModule, int temperature, int minTemp, int maxTemp)
//     : id(id), diModule(diModule), temperature(temperature), minTemp(minTemp), maxTemp(maxTemp) {}

//   void listen() {
//     byte state = this->diModule.getState();

//     if (state != this->prevState) {
//       this->prevState = state;
//       if (state == 1) {
//         if (this->temperature < this->minTemp || this->temperature >= this->maxTemp) {
//           this->temperature = this->minTemp;
//         } else {
//           this->temperature += 10;
//         }
//       }
//     }
//   }

//   void displayTemp(AnalogInput &aiModule) {
//     aiModule.value = this->temperature;
//   }

//   ~TempSwitch(){};
// };


DigitalInput &running = digitalInputs[0];
DigitalInput &fault = digitalInputs[2];
DigitalInput &waterLevelLow = digitalInputs[3];
DigitalInput &waterLevelHigh = digitalInputs[4];

DigitalOutput &relay1 = digitalOutputs[0];
DigitalOutput &relay2 = digitalOutputs[1];
DigitalOutput &relay3 = digitalOutputs[2];

AnalogInput1024 &temp1 = analogInputs[0];
AnalogInput1024 &temp2 = analogInputs[1];
AnalogInput1024 &temp3 = analogInputs[2];
AnalogInput1024 &ev1 = analogInputs[6];
AnalogInput1024 &ev2 = analogInputs[7];
AnalogInput1024 &ev3 = analogInputs[8];
AnalogInput1024 &ev4 = analogInputs[9];
AnalogInput1024 &ev5 = analogInputs[10];
AnalogInput1024 &ev6 = analogInputs[11];

AnalogOutput &ao1 = analogOutputs[0];
AnalogOutput &ao2 = analogOutputs[1];
AnalogOutput &ao3 = analogOutputs[2];

class SubSystem {
public:
  SubSystem(void) {
    relay1.cut();
    relay2.cut();
    relay3.cut();
  }

  void loop() {
    running.listen();
    fault.listen();
    waterLevelLow.listen();
    waterLevelHigh.listen();

    temp1.listen();
    temp2.listen();
    temp3.listen();

    int actualTemp1 = readingToActualTemp(temp1.getValue());
    int actualTemp2 = readingToActualTemp(temp2.getValue());
    int actualTemp3 = readingToActualTemp(temp3.getValue());
    int aoValue1 = getAoValue(actualTemp1);
    int aoValue2 = getAoValue(actualTemp2);
    int aoValue3 = getAoValue(actualTemp3);

    if (actualTemp1 > 800) {
      relay1.connect();
    } else {
      relay1.cut();
    }

    if (actualTemp2 > 250) {
      relay2.connect();
    } else {
      relay2.cut();
    }

    if (actualTemp3 > 250) {
      relay3.connect();
    } else {
      relay3.cut();
    }

    ao1.set(aoValue1);
    ao2.set(aoValue2);
    ao3.set(aoValue3);
  }

  ~SubSystem() {}
};

#endif