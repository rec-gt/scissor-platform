#include "Globals.h"

class AnalogInput {
private:
  byte pin;
  unsigned int value;

public:
  AnalogInput(byte pin)
    : pin(pin) {
    pinMode(pin, INPUT);
  }

  void listen() {
    unsigned long sum = 0;

    for (int i = 0; i < OVERSAMPLING_FACTOR; i++) {
      sum += analogRead(this->pin);
    }

    this->value = sum >> SHIFT_BITS;

    Serial.println(this->value);
  }

  unsigned int getValue() {
    return this->value;
  }
};