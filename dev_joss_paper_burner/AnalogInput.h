#include "Globals.h"

class AnalogInput {
private:
  byte pin;
  uint16_t value;

public:
  AnalogInput(byte pin)
    : pin(pin) {
    pinMode(pin, INPUT);
  }

  void listen() {
    uint32_t sum = 0;

    for (int i = 0; i < OVERSAMPLING_FACTOR; i++) {
      sum += analogRead(this->pin);
    }

    this->value = (sum / OVERSAMPLING_FACTOR) << SHIFT_BITS;

    // this->value = sum >> SHIFT_BITS;

    // this->value = analogRead(this->pin);
  }

  unsigned int getValue() {
    return this->value;
  }
};