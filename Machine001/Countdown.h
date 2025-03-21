#include "Arduino.h"
#include "DisplayOLED.h"

#ifndef countdown_h
#define countdown_h

class CountdownTimer {
private:
  int lastRemainingTime = 10;
  unsigned long startMillis = millis();
  unsigned long duration = 10 * 1000;

public:
  CountdownTimer() {}

  void set() {
    startMillis = millis();
  }

  void countdown(void (*callback)()) {
    unsigned long currMillis = millis();

    int remainingTime = (duration - (currMillis - startMillis)) / 1000;

    if (remainingTime != lastRemainingTime) {
      this->lastRemainingTime = remainingTime;

      // slow update
      if (remainingTime >= 0) {
        char* cArr[] = { "允許運作",
                         utils.num2Char(remainingTime + 1),
                         "秒" };
        char* c = utils.concatCharN(cArr, (sizeof(cArr) / sizeof(cArr[0])));
        displayOLED.print("", c, "", (300 + remainingTime));
      }
    }

    if (currMillis - startMillis >= duration) {
      this->lastRemainingTime = 10;
      callback();
    }
  }
};

#endif
