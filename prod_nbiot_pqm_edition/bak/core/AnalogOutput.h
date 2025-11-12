#ifndef AnalogOutput_H
#define AnalogOutput_H

class AnalogOutput {
private:
  byte pin;

public:
  uint16_t value;

  AnalogOutput() {}

  AnalogOutput(byte pin)
    : pin(pin) {
    pinMode(pin, OUTPUT);
  }

  void set(uint16_t value) {
    this->value = value;
    analogWrite(this->pin, this->value);
  }

  uint16_t getValue() {
    return this->value;
  }
};

#endif