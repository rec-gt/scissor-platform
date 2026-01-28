#ifndef SubSystem_H
#define SubSystem_H
#define SET_TEMP 60

#include "./SubGlobals.h"
Timer deviceTimer(10000UL);

int temp1 = 25;
int temp2 = 25;
int temp3 = 25;
int temp4 = 25;
int temp5 = 25;
int temp6 = 25;

int tempHistory1[10] = {};
int tempHistory2[10] = {};
int tempHistory3[10] = {};
int tempHistory4[10] = {};
int tempHistory5[10] = {};
int tempHistory6[10] = {};

DigitalOutput &relay1 = digitalOutputs[0];
DigitalOutput &relay2 = digitalOutputs[1];

AnalogInput &kps1 = analogInputs[0];
AnalogInput &kps2 = analogInputs[1];
AnalogInput &kps3 = analogInputs[2];
AnalogInput &kps4 = analogInputs[3];
AnalogInput &kps5 = analogInputs[4];
AnalogInput &kps6 = analogInputs[5];

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
    }

    kps1.set(holdingRegisterValues[0]);
    kps2.set(holdingRegisterValues[1]);
    kps3.set(holdingRegisterValues[2]);
    kps4.set(holdingRegisterValues[3]);
    kps5.set(holdingRegisterValues[4]);
    kps6.set(holdingRegisterValues[5]);

    // if (this->isOverHeat()) {
    //   relay1.cut();
    //   relay2.connect();
    //   iot.forcePublish();
    // } else {
    //   relay1.connect();
    //   relay2.cut();
    // }
  }

  ~SubSystem() {}
};

#endif