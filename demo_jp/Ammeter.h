#include "Arduino.h"

class Ammeter {
private:
  byte pin;
  int current = 0;  // 100=1A, 1000=10A

public:
  Ammeter(byte pin)
    : pin(pin) {
    pinMode(pin, INPUT);
  }

  void listen() {
    unsigned long avg = 0;
    for (size_t i = 0; i < 64; i++) {
      avg += analogRead(this->pin);
    };

    this->current = map((avg / 64.), 204.6, 1023, 0, 1000);

    // hardcoded, can remove this
    if (this->current > 0 && this->current < 20) {
      this->current = random(10, 25);
    }
  }

  int get() {
    if (this->current < 0) {
      return 0;
    }
    return this->current;
  }
};