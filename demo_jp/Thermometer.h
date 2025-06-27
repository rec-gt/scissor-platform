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
    this->temperature = map(analogRead(this->pin), 204.6, 1023, 0, 20000);
  }

  int get() {
    return this->temperature;
  }
};