#ifndef AnalogOutput_H
#define AnalogOutput_H

class AnalogOutput {
private:
  byte pin;

public:
  byte value;

  AnalogOutput() {}

  AnalogOutput(byte pin)
    : pin(pin) {
    pinMode(pin, OUTPUT);
  }

  void set(byte value) {
    this->value = value;
    analogWrite(this->pin, this->value);
  }

  byte getValue() {
    return this->value;
  }
};

#endif