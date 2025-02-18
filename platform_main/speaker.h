#include "Arduino.h"

class Speaker {
private:
  byte pin;

public:
  Speaker(byte pin)
    : pin(pin) {
    pinMode(pin, OUTPUT);
  }

  void on() {
    digitalWrite(pin, HIGH);
  }

  void off() {
    digitalWrite(pin, LOW);
  }
};