#include "Arduino.h"

#ifndef analog_input_h
#define analog_input_h

class AnalogInput {
private:
  byte pin;
  float reading = 0;
  float voltage = 0;

public:
  AnalogInput(byte pin)
    : pin(pin) {
    pinMode(this->pin, INPUT);
  }

  float avgRead() {
    unsigned long avg = 0;
    for (size_t i = 0; i < 10; i++) {
      avg += analogRead(this->pin);
    };
    return avg / 10.;
  }

  float calVoltage() {
    return this->reading / 1023. * 5.;
  }

  void listen() {
    this->reading = this->avgRead();
    this->voltage = this->calVoltage();
  }


  float getReading() {
    return this->reading;
  }

  float getVoltage() {
    return this->voltage;
  }
};

extern AnalogInput analogInput;

#endif
