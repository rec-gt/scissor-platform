#include "Arduino.h"

class AnalogInput {
private:
  byte pin;
  float reading = 0;
  float voltage = 0;

  float avgRead() {
    float avg = 0.;
    for (size_t i = 0; i < 32; i++) {
      avg += analogRead(this->pin);
    };
    return avg / 32.;
  }

  float calVoltage() {
    return this->reading / 1023. * 5.;
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

  void print() {
    Serial.print("Pin: ");
    Serial.print(this->pin);
    Serial.print(" Reading: ");
    Serial.print(this->getReading());
    Serial.print(" Voltage: ");
    Serial.print(this->getVoltage());
    Serial.println();
  }

  ~AnalogInput(){};
};
