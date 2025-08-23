#include "Arduino.h"

class Thermometer {
private:
  byte pin;
  int temperature = 2500;  //25

public:
  Thermometer(byte pin)
    : pin(pin) {
    pinMode(pin, INPUT);
  }

  void listen() {
    unsigned long avg = 0;
    for (size_t i = 0; i < 64; i++) {
      avg += analogRead(this->pin);
    };
    this->temperature = map((avg / 64.), 204.6, 1023, 0, 20000);
  }

  int get() {
    return this->temperature;
  }
};