#include "Arduino.h"

// Relay default is LOW, and is NC, LOW=NC, HIGH=NO
class Relay {
private:
  byte pin;

public:
  Relay(byte pin)
    : pin(pin) {
    pinMode(pin, OUTPUT);
  }

  void connect() {
    digitalWrite(pin, HIGH);
  }

  void cut() {
    digitalWrite(pin, LOW);
  }
};