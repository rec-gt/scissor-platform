#ifndef KPSManager_H
#define KPSManager_H
#define HISTORY_SIZE 3

#include "./SubGlobals.h"

Timer kpsMngrTimer(10000UL);

class KPSManager {
private:
  KPS sensors[MAX_TEMPERATURE_CHANNEL_SIZE] = {};

public:
  KPSManager(void) {}

  void init() {
    for (byte i = 0; i < TEMPERATURE_CHANNEL_SIZE; i++) {
      this->sensors[i] = KPS();
    }
  }

  void read() {
    mbRtuClient.requestFrom(1, HOLDING_REGISTERS, 0, PARAMETERS_SIZE);

    for (size_t i = 0; i < TEMPERATURE_CHANNEL_SIZE; i++) {
      holdingRegisterValues[i] = (uint32_t)mbRtuClient.read();
      this->sensors[i].set(holdingRegisterValues[i]);
    }
  }

  void check() {
    bool flag = true;  // flag = true 等於系統正常

    for (size_t i = 0; i < TEMPERATURE_CHANNEL_SIZE; i++) {
      if (holdingRegisterValues[i] > 10000) {
        flag = false;
      }
    }

    sysMonitor.setHealth(flag ? SUBSYS_HEALTHY : SUBSYS_FAILURE);
  }

  void loop() {
    if (kpsMngrTimer.autoTimeout(1000)) {
      this->read();
      // this->debug();
    }
  }

  bool areAllValid() {
    for (byte i = 0; i < TEMPERATURE_CHANNEL_SIZE; i++) {
      if (this->sensors[i].isValid() == false) {
        return false;
      };
    }

    return true;
  }

  bool anyOverheat() {
    byte flag = TEMPERATURE_CHANNEL_SIZE;  // TEMPERATURE_CHANNEL_SIZE = assume all overheat by default

    for (byte i = 0; i < TEMPERATURE_CHANNEL_SIZE; i++) {
      if (this->sensors[i].isOverheat(THRESHOLD_DANGEROUS) == false) {
        flag--;  // prove that each sensor is NOT overheated
      };
    }

    if (flag == 0) {
      return false;  // false = no one overheat
    }

    return true;
  }

  bool allSafe() {
    byte flag = 0;  // 0 = assume not all safe

    for (byte i = 0; i < TEMPERATURE_CHANNEL_SIZE; i++) {
      if (this->sensors[i].isSafe(THRESHOLD_SAFE)) {
        flag++;  // prove that each sensor is safe
      };
    }

    if (flag == TEMPERATURE_CHANNEL_SIZE) {
      return true;
    }

    return false;
  }

  uint16_t getValue(byte i) {
    if (i <= TEMPERATURE_CHANNEL_SIZE) {
      return this->sensors[i].getReading();
    }
    return 0;
  }

  void debug() {
    for (byte i = 0; i < TEMPERATURE_CHANNEL_SIZE; i++) {
      Serial.println(this->sensors[i].getReading());
    }
  }

  ~KPSManager() {}
};

#endif