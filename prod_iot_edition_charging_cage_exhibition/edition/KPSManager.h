#ifndef KPSManager_H
#define KPSManager_H
#define HISTORY_SIZE 3

#include "./SubGlobals.h"

Timer kpsMngrTimer(10000UL);

class KPSManager {
private:
  KPS sensors[TEMPERATURE_CHANNEL_SIZE] = {};

public:
  KPSManager(void) {}

  void init() {
    for (byte i = 0; i < TEMPERATURE_CHANNEL_SIZE; i++) {
      this->sensors[i] = KPS();
    }
  }

  void read() {
    for (byte i = 0; i < TEMPERATURE_CHANNEL_SIZE; i++) {
      this->sensors[i].set(analogInputs[i].value);
    }
  }

  void loop() {
    if (kpsMngrTimer.autoTimeout(500)) {
      this->read();
    }
  }

  bool anyOverheat() {
    byte flag = THRESHOLD_DANGEROUS;  // THRESHOLD_DANGEROUS = assume all overheat by default

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
      if (this->sensors[i].isSafe(THRESHOLD_DANGEROUS) == false) {
        flag++;  // prove that each sensor is safe
      };
    }

    if (flag == TEMPERATURE_CHANNEL_SIZE) {
      return true;
    }

    return false;
  }

  void debug() {
    for (byte i = 0; i < TEMPERATURE_CHANNEL_SIZE; i++) {
      Serial.println(this->sensors[i].getReading());
    }
  }

  ~KPSManager() {}
};

#endif