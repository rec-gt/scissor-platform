#include "Arduino.h"

class ContactLine {
private:
  char* name;
  byte pin;
  byte state;
public:
  ContactLine(char* name, byte pin)
    : name(name), pin(pin) {
    pinMode(pin, INPUT_PULLUP);
  }

  void listen() {
    byte reading = digitalRead(pin);
    this->state = reading;
  }

  char* getName() {
    return this->name;
  }

  byte getState() {
    return this->state;
  }

  bool isBreaked() {
    return this->state == HIGH;
  }
};