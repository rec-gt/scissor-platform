#include "Arduino.h"

class WarningSystem {
private:
  byte pin;

public:
  WarningSystem(byte pin)
    : pin(pin) {
    pinMode(pin, OUTPUT);
    this->off();
  }

  void on() {
    digitalWrite(pin, HIGH);
  }

  void off() {
    digitalWrite(pin, LOW);
  }
};