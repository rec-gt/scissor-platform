#include "Arduino.h"

class BaseThresholdSwitch {
private:
  byte pin;
  byte state;
  byte previousState;
public:
  BaseThresholdSwitch(byte pin)
    : pin(pin) {
    pinMode(pin, INPUT);
  }

  void listen() {
    state = digitalRead(pin);
  }

  bool isOn() {
    return state == HIGH;
  }
};