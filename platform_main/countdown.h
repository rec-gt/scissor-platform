#include "Arduino.h"
#include "displayOLED.h"

class CountdownTimer {
private:
  int remainingTime = 10;
  unsigned long startMillis = millis();
  unsigned long duration = 10 * 1000;

public:
  CountdownTimer(unsigned long duration)
    : duration(duration * 1000) {
  }

  void set() {
    startMillis = millis();
  }

  void countdown(DisplayOLED displayOLED, void (*callback)()) {
    unsigned long currMillis = millis();

    // print countdown
    int currRemainingTime = (duration - (currMillis - startMillis)) / 1000;

    if (currRemainingTime >= 0 && currRemainingTime != remainingTime) {
      remainingTime = currRemainingTime;
      
      char cstr[16];
      displayOLED.print("", "暫時運作", itoa(remainingTime+1, cstr, 10));
      // Serial.println(int(currRemainingTime) + 1);
    }

    if (currMillis - startMillis >= duration) {
      remainingTime = duration / 1000;
      callback();
    }
  }
};