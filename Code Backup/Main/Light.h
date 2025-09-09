#include "Arduino.h"

class Light {
private:
  byte pin;

public:
  Light(byte pin)
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