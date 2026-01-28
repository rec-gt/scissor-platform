#ifndef SubSystem_H
#define SubSystem_H
#define SET_TEMP 60

#include "./SubGlobals.h"
#include "./KPS.h"

Timer deviceTimer(10000UL);

uint16_t setTemp = 600;
KPS kps1;
KPS kps2;
KPS kps3;
KPS kps4;
KPS kps5;
KPS kps6;

DigitalOutput &relay1 = digitalOutputs[0];
DigitalOutput &relay2 = digitalOutputs[1];

class SubSystem {
private:
  void readIn1000ms() {
    mbRtuClient.requestFrom(1, HOLDING_REGISTERS, 0, PARAMETERS_SIZE);

    for (size_t i = 0; i < PARAMETERS_SIZE; i++) {
      holdingRegisterValues[i] = (uint32_t)mbRtuClient.read();
      Serial.println(holdingRegisterValues[i]);
    }
  }

public:
  SubSystem(void) {}

  void init() {
    if (!mbRtuClient.begin(9600)) {
      Serial.println(F("Failed to start Modbus RTU Client!"));
      while (1) {};
    }
  }

  void loop() {
    if (deviceTimer.autoTimeout(1000)) {
      this->readIn1000ms();

      kps1.set(holdingRegisterValues[0]);
      kps2.set(holdingRegisterValues[1]);
      kps3.set(holdingRegisterValues[2]);
      kps4.set(holdingRegisterValues[3]);
      kps5.set(holdingRegisterValues[4]);
      kps6.set(holdingRegisterValues[5]);
    }

    if (kps1.isOverheat(setTemp) || kps2.isOverheat(setTemp) || kps3.isOverheat(setTemp) || kps4.isOverheat(setTemp) || kps5.isOverheat(setTemp) || kps6.isOverheat(setTemp)) {
      relay1.cut();
      relay2.connect();
      iot.forcePublish();
    } else {
      relay1.connect();
      relay2.cut();
    }
  }

  ~SubSystem() {}
};

#endif