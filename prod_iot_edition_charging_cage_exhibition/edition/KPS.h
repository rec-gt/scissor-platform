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
    for (int i = 0; i < HISTORY_SIZE; i++) {
      if (this->readingHistory[i] >= setTemp) {
        return true;  // 但凡其中一個sample過熱，都算過熱
      }
    }
    return false;
  }

  bool isSafe(uint16_t setTemp) {  // 所有sample都低溫，才算低溫
    bool flag = false;  // flag == true 等於低溫
    uint8_t counter = 0;

    for (int i = 0; i < HISTORY_SIZE; i++) {
      if (this->readingHistory[i] < setTemp) {
        counter++;
      }
    }

    if (counter == HISTORY_SIZE) {
      flag = true;
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