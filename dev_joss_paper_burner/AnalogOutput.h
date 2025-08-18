#include "Globals.h"

#ifndef AnalogOutput_H
#define AnalogOutput_H

class AnalogOutput {
private:
  byte pin;
  unsigned int value;

public:
  AnalogOutput() {}

  AnalogOutput(byte pin)
    : pin(pin) {
    pinMode(pin, OUTPUT);
  }

  void set(unsigned int value) {
    analogWrite(this->pin, value);
  }
};

#endif