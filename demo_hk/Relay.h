#include "Arduino.h"

class Relay {
private:
  byte pin;

public:
  Relay(byte pin)
    : pin(pin) {
    pinMode(pin, OUTPUT);
    this->cut();
  }

  void connect() {
    digitalWrite(pin, HIGH);
  }

  void cut() {
    digitalWrite(pin, LOW);
  }
};