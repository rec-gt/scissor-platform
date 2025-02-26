#include "Arduino.h"

class WarningSystem {
private:
  byte pin;

public:
  WarningSystem(byte pin)
    : pin(pin) {
    pinMode(pin, OUTPUT);
  }

  void on() {
    digitalWrite(pin, LOW);
  }

  void off() {
    digitalWrite(pin, HIGH);
  }
};