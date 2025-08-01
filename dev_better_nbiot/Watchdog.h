#include <avr/wdt.h>

#ifndef Watchdog_h
#define Watchdog_h

class Watchdog {
private:
  typedef void (*CallbackFunction)(void);

  unsigned long timeout = 1UL * 60UL * 1000UL;
  unsigned long prevMillis = 0;
  bool _enable = false;
  CallbackFunction callback;

public:
  Watchdog(unsigned long timeout) {
    this->timeout = timeout;
    this->callback = nullptr;
  }


  void setCallback(void (*callback)()) {
    this->callback = callback;
  }

  void reboot(void) {
    if (this->callback != nullptr) {
      this->callback();
      this->pet();
    } else {
      // reboot whole arduino
      wdt_enable(WDTO_15MS);
      while (1) {};
    }
  }

  void monitor(void) {
    if (this->_enable) {
      if (millis() - this->prevMillis > this->timeout) {
        this->reboot();
      }
    }
  }

  void enable(void) {
    this->_enable = true;
  }

  void disable(void) {
    this->_enable = false;
  }

  void pet(void) {
    this->prevMillis = millis();
  };

  ~Watchdog(){};
};

#endif
