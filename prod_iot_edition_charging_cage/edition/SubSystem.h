#ifndef SubSystem_H
#define SubSystem_H
#define HISTORY_SIZE 10

#include "./SubGlobals.h"

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
  int readingToActualTemp(int reading) {
    return map(constrain(reading, 196, 1023), 196, 1023, 0, 1300);
  }

  bool isOverHeat() {
    return (temp1 > 60 || temp2 > 60 || temp3 > 60 || temp4 > 60 || temp5 > 60 || temp6 > 60);
  }

  void updateTempHistory(int (&history)[HISTORY_SIZE], int temp) {
    for (int i = 1; i < HISTORY_SIZE; i++) {
      history[i - 1] = history[i];
    }
    history[HISTORY_SIZE - 1] = temp;
  }

public:
  SubSystem(void) {}

  void init() {
    configAnalogInputResolution(0);
  }

  void loop() {
    kps1.listen();
    kps2.listen();
    kps3.listen();
    kps4.listen();
    kps5.listen();
    kps6.listen();

    temp1 = readingToActualTemp(kps1.getValue());
    temp2 = readingToActualTemp(kps2.getValue());
    temp3 = readingToActualTemp(kps3.getValue());
    temp4 = readingToActualTemp(kps4.getValue());
    temp5 = readingToActualTemp(kps5.getValue());
    temp6 = readingToActualTemp(kps6.getValue());

    this->updateTempHistory(tempHistory1, temp1);
    this->updateTempHistory(tempHistory2, temp2);
    this->updateTempHistory(tempHistory3, temp3);
    this->updateTempHistory(tempHistory4, temp4);
    this->updateTempHistory(tempHistory5, temp5);
    this->updateTempHistory(tempHistory6, temp6);

    if (this->isOverHeat()) {
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