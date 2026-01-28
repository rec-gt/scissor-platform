#ifndef KPS_H
#define KPS_H
#define HISTORY_SIZE 3

#include "./SubGlobals.h"

class KPS {
private:
  uint16_t reading = 250;
  uint16_t readingHistory[HISTORY_SIZE] = {};

  void appendTempHistory(uint16_t (&history)[HISTORY_SIZE], uint16_t temp) {
    for (int i = 1; i < HISTORY_SIZE; i++) {
      history[i - 1] = history[i];
    }
    history[HISTORY_SIZE - 1] = temp;
  }

public:
  KPS(void) {}

  void set(uint16_t reading) {
    this->reading = reading;
    this->appendTempHistory(this->readingHistory, this->reading);
  }


  void debug() {
    for (int i = 0; i < HISTORY_SIZE; i++) {
      Serial.println(this->readingHistory[i]);
    }
  }

  bool isOverheat(uint16_t setTemp) {
    bool flag = false;  // flag == true 等於過熱
    for (int i = 0; i < HISTORY_SIZE; i++) {
      flag = this->readingHistory[i] > setTemp;
    }

    return flag;
  }

  bool isSafe(uint16_t setTemp) {
    bool flag = false;  // flag == true 等於低溫
    for (int i = 0; i < HISTORY_SIZE; i++) {
      flag = this->readingHistory[i] < setTemp;
    }

    return flag;
  }

  bool isFailure() {
    bool flag = false;  // flag == true 等於異常
    for (int i = 0; i < HISTORY_SIZE; i++) {
      flag = this->readingHistory[i] > 20000;  // 異常讀數
    }

    return flag;
  }

  ~KPS() {}
};

#endif