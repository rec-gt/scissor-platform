#include "Arduino.h"
#include "displayOLED.h"

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

  void countdown(DisplayOLED displayOLED, void (*callback)()) {
    unsigned long currMillis = millis();

    int remainingTime = (duration - (currMillis - startMillis)) / 1000;

    if (remainingTime != lastRemainingTime) {
      this->lastRemainingTime = remainingTime;

      // slow update
      if (remainingTime >= 0) {
        char* c1 = displayOLED.concatChar("允許暫時運作", displayOLED.num2Char(remainingTime + 1));
        char* c2 = displayOLED.concatChar(c1, "秒");
        displayOLED.print("", c2, "", 301);
      }
    }

    if (currMillis - startMillis >= duration) {
      this->lastRemainingTime = 10;
      callback();
    }
  }
};

#endif
