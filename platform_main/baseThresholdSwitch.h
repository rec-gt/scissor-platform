#include "Arduino.h"

class BaseThresholdSwitch {
private:
  byte pin;
  byte state;
public:
  BaseThresholdSwitch(byte pin)
    : pin(pin) {
    pinMode(pin, INPUT);
  }

  void listen() {
    this->state = digitalRead(this->pin);
  }

  bool isOn() {
    return this->state == HIGH;
  }
};