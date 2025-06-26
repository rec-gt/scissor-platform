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

  void read() {
    this->current = map(analogRead(this->pin), 204.6, 1023, 0, 1000);
    Serial.println(this->current / 100.);
  }

  int get() {
    return this->current;
  }
};