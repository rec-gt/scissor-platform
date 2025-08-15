#include "Globals.h"

class AnalogOutput {
private:
  byte pin;
  unsigned int value;

public:
  AnalogOutput(byte pin)
    : pin(pin) {
    pinMode(pin, OUTPUT);
  }

  void set(unsigned int value) {
    analogWrite(this->pin, value);
  }
};