#include "Arduino.h"

class Ammeter {
private:
  byte pin;
  int current = 1;

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