#include "Arduino.h"

class CountdownTimer {
private:
  int remainingTime = 10;
  unsigned long startMillis = millis();
  unsigned long duration = 10 * 1000;

public:
  CountdownTimer(unsigned long duration)
    : duration(duration * 1000) {
  }

  void setStart(unsigned long startMillis) {
    this->startMillis = startMillis;
  }

  void countdown(void (*callback)()) {
    unsigned long currentMillis = millis();

    // print countdown
    int currentRemainingTime = (this->duration - (currentMillis - this->startMillis)) / 1000;

    if (currentRemainingTime >= 0 && currentRemainingTime != this->remainingTime) {
      this->remainingTime = currentRemainingTime;
      Serial.println(int(currentRemainingTime) + 1);
    }

    if (currentMillis - startMillis >= this->duration) {
      this->remainingTime = this->duration / 1000;
      callback();
    }
  }
};