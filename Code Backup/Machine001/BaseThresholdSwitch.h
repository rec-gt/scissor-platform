#include "Arduino.h"

class BaseThresholdSwitch {
private:
  byte pin;
  byte state = HIGH;
  byte lastState = HIGH;
  unsigned long lastMillis;
public:
  BaseThresholdSwitch(byte pin)
    : pin(pin) {
    pinMode(this->pin, INPUT_PULLUP);
    digitalWrite(this->pin, HIGH);
  }

  void listen() {
    byte reading = digitalRead(this->pin);
    
    if (reading != this->lastState) {
      if ((millis() - this->lastMillis) > 50) {
        this->state = reading;
        this->lastState = reading;
      }
    } else {
      this->lastMillis = millis();
    }
  }

  bool on() {  // on = longer threshold
    return this->state == HIGH;
  }
};