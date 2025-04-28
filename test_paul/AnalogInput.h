#include "Arduino.h"

class AnalogInput {
private:
  byte pin;
  float reading = 0;
  float voltage = 0;

  float avgRead() {
    unsigned long avg = 0;
    for (size_t i = 0; i < 10; i++) {
      avg += analogRead(this->pin);
    };
    return avg / 10.;
  }

  float calVoltage() {
    return this->reading / 1023. * 3.3;
  }

public:
  AnalogInput() {}

  AnalogInput(byte pin)
    : pin(pin) {
    pinMode(this->pin, INPUT);
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

  ~AnalogInput(){};
};
