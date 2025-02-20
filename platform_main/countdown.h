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

      char* c1 = displayOLED.concatChar("暫時運作: ", displayOLED.num2Char(remainingTime + 1));
      char* c2 = displayOLED.concatChar(c1, "s");
      displayOLED.print("", c2, "", remainingTime + 1);
    }

    if (currMillis - startMillis >= duration) {
      remainingTime = duration / 1000;
      callback();
    }
  }
};