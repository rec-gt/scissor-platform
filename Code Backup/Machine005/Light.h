#include "Arduino.h"

class Light {
private:
  byte pin;

public:
  Light(byte pin)
    : pin(pin) {
    pinMode(pin, OUTPUT);
    this->off();
  }

  void on() {
    digitalWrite(pin, HIGH);
  }

  void off() {
    digitalWrite(pin, LOW);
  }

  void blink() {
    
  }
};