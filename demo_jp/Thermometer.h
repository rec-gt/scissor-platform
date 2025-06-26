#include "Arduino.h"

class Thermometer {
private:
  byte pin;

public:
  Thermometer(byte pin)
    : pin(pin) {
    pinMode(pin, INPUT);
  }

  void read() {
    Serial.println(map(analogRead(this->pin), 204.6, 1023, 0, 20000) / 100.);
  }
};