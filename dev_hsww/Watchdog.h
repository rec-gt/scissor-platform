#include <avr/wdt.h>

#ifndef Watchdog_h
#define Watchdog_h

class Watchdog {
private:
  unsigned long timeout = 3UL * 60UL * 1000UL;
  unsigned long prevMillis = 0;

public:
  Watchdog() {
    this->prevMillis = millis();
  };

  void listen() {
    if (millis() - this->prevMillis > this->timeout) {
      this->reset();
    }
  }

  void feed() {
    this->prevMillis = millis();
  }

  void reset(void) {
    wdt_enable(WDTO_15MS);
    while (1) {}
  }

  ~Watchdog(){};
};

extern Watchdog watchdog;

#endif