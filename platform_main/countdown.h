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
    startMillis = startMillis;
  }

  void countdown(void (*callback)()) {
    unsigned long currentMillis = millis();

    // print countdown
    int currRemainingTime = (duration - (currentMillis - startMillis)) / 1000;

    if (currRemainingTime >= 0 && currRemainingTime != remainingTime) {
      remainingTime = currRemainingTime;
      Serial.println(int(currRemainingTime) + 1);
    }

    if (currentMillis - startMillis >= duration) {
      remainingTime = duration / 1000;
      callback();
    }
  }
};