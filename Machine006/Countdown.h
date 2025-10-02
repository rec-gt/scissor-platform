#include "Arduino.h"
// #include "DisplayOLED.h"

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

    if (currMillis - this->startMillis >= this->duration) {
      this->lastRemainingTime = 10;
      callback();
    }
  }

  ~CountdownTimer(){};
};

#endif
