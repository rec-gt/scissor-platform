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
    this->current = map(analogRead(this->pin), 204.6, 1023, 0, 1000);
  }

  int get() {
    if (this->current < 0) {
      return 0;
    }
    return this->current;
  }
};