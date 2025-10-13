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

Relay relay1(0);
Relay relay2(1);
Relay relay3(2);

class SubSystem {
private:
  int readingToActualTemp(int reading) {
    if (reading < 204) {
      return 0;
    } else {
      return map(reading, 196, 1023, 0, 1300);
    }
  }

  int breakPoint1 = 372;  // value in reading, 372 (800 degree C)
  int breakPoint2 = 234;  // value in reading

  bool prevBtnState = false;

public:
  SubSystem(void) {
    relay1.forceConnect();
    relay2.forceConnect();
    relay3.forceConnect();
  }

  void loop() {
    this->handle800Temp();
    this->handleConfigurableTemp();
    this->handleChangeConfigTemp();
  }

  void handle800Temp() {
    int reading = analogInputs[0].getValue();
    int actualTemp = this->readingToActualTemp(reading);
    int aoValue = map(actualTemp, 0, 1300, 0, 255);

    Serial.println(reading);
    Serial.println(actualTemp);
    Serial.println(aoValue);

    // === display actual temperature ===
    analogOutputs[0].set(aoValue);

    // === logic control ===
    if (actualTemp >= 60) {
      relay1.cut();
    } else {
      relay1.debounceConnect();
    }
  }

  void handleConfigurableTemp() {
    int reading = analogInputs[1].getValue();
    int actualTemp = this->readingToActualTemp(reading);
    int aoValue = map(actualTemp, 0, 1300, 0, 255);


    // === display actual temperature ===
    analogOutputs[1].set(aoValue + 2);
    analogInputs[5].value = (this->breakPoint2) * 3 - 306;

    // === logic control ===
    if (reading >= this->breakPoint2) {
      digitalOutputs[1].cut();
    }

    if (reading <= this->breakPoint2 - 4) {
      digitalOutputs[1].connect();
    }
  }


  void handleChangeConfigTemp() {
    digitalInputs[6].listen();
    bool state = digitalInputs[6].getState();

    if (this->prevBtnState != state) {  // state change detected
      this->prevBtnState = state;
      if (state == 0) {
        this->breakPoint2 += 4;
        if (this->breakPoint2 >= 254) {
          this->breakPoint2 = 234;
        }
      }
    }
  }

  ~SubSystem() {}
};

#endif