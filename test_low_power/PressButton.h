#include "Arduino.h"

class PressButton {
private:
  byte pin;
  byte state;
  unsigned long lastMillis;
public:
  PressButton(byte pin)
    : pin(pin) {
    pinMode(pin, INPUT_PULLUP);
  }

  void listen() {
    byte reading = digitalRead(pin);

    if (reading == LOW) {
      if ((millis() - this->lastMillis) > 50) {
        this->state = reading;
      }
    } else {
      this->state = HIGH;
      this->lastMillis = millis();
    }
  }

  bool isPressed() {
    return this->state == LOW;
  }
};