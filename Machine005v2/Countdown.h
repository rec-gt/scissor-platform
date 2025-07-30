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
  CountdownTimer(){};

  void set() {
    this->startMillis = millis();
  }

  void countdown(void (*callback)()) {
    unsigned long currMillis = millis();

    int remainingTime = (this->duration - (currMillis - this->startMillis)) / 1000;

    // if (remainingTime != this->lastRemainingTime) {
    //   this->lastRemainingTime = remainingTime;

    //   // slow update
    //   if (remainingTime >= 0) {
    //     char* cArr[] = {
    //       "允許運作",
    //       utils.num2Char(remainingTime + 1),
    //       "秒"
    //     };
    //     char* c = utils.concatCharN(cArr, 3);
    //     displayOLED.print("", c, "", (DISPLAY_SYS_COUNTDOWN_0 + remainingTime));
    //   }
    // }

    if (currMillis - this->startMillis >= this->duration) {
      this->lastRemainingTime = 10;
      callback();
    }
  }

  ~CountdownTimer(){};
};

#endif
