#ifndef SubSystem_H
#define SubSystem_H
#define SET_TEMP 60

#include "./SubGlobals.h"
#include "./KPS.h"

Timer deviceTimer(10000UL);

uint16_t THRESHOLD_DANGEROUS = 330;
uint16_t THRESHOLD_SAFE = 450;

KPS kps1;
KPS kps2;
KPS kps3;
KPS kps4;
KPS kps5;
KPS kps6;

DigitalOutput &powerRelay = digitalOutputs[0];
DigitalOutput &lightRelay = digitalOutputs[1];

class SubSystem {
private:
  enum SUBSYS_STATUS {
    SUBSYS_RUNNING,
    SUBSYS_STOPPED,
    SUBSYS_FAILURE,
  };

  byte status = SUBSYS_RUNNING;

  void valueChecker() {
    bool flag = true;  // flag = true 等於系統正常

    for (size_t i = 0; i < PARAMETERS_SIZE; i++) {
      if (holdingRegisterValues[i] > 2000 * 10) {
        flag = false;
      }
    }

    this->status = flag ? SUBSYS_RUNNING : SUBSYS_FAILURE;
  }

  void readIn1000ms() {
    mbRtuClient.requestFrom(1, HOLDING_REGISTERS, 0, PARAMETERS_SIZE);

    for (size_t i = 0; i < PARAMETERS_SIZE; i++) {
      holdingRegisterValues[i] = (uint32_t)mbRtuClient.read();
    }

    this->valueChecker();
  }

public:
  SubSystem(void) {}

  void init() {
    if (!mbRtuClient.begin(9600)) {
      Serial.println(F("Failed to start Modbus RTU Client!"));
      this->status = SUBSYS_FAILURE;
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

    if (this->status == SUBSYS_RUNNING) {
      // logic
      if (kps1.isOverheat(THRESHOLD_DANGEROUS)
          || kps2.isOverheat(THRESHOLD_DANGEROUS)
          || kps3.isOverheat(THRESHOLD_DANGEROUS)
          || kps4.isOverheat(THRESHOLD_DANGEROUS)
          || kps5.isOverheat(THRESHOLD_DANGEROUS)
          || kps6.isOverheat(THRESHOLD_DANGEROUS)) {
        this->status = SUBSYS_STOPPED;
      }
      Serial.println(F("SUBSYS_RUNNING"));

      // control
      powerRelay.connect();
      lightRelay.cut();
    }

    if (this->status == SUBSYS_STOPPED) {
      // logic
      if (kps1.isSafe(THRESHOLD_SAFE)
          || kps2.isSafe(THRESHOLD_SAFE)
          || kps3.isSafe(THRESHOLD_SAFE)
          || kps4.isSafe(THRESHOLD_SAFE)
          || kps5.isSafe(THRESHOLD_SAFE)
          || kps6.isSafe(THRESHOLD_SAFE)) {
        this->status = SUBSYS_RUNNING;
      }

      Serial.println(F("SUBSYS_STOPPED"));

      // control
      powerRelay.cut();
      lightRelay.connect();
    }

    if (this->status == SUBSYS_FAILURE) {
      Serial.println(F("SYSTEM FAILURE"));
      powerRelay.cut();
      lightRelay.connect();
    }

    kps1.debug();
  }


  ~SubSystem() {}
};

#endif