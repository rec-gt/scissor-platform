#include "Arduino.h"

#ifndef PressButton_h
#define PressButton_h

#define PRESS_BUTTON_PIN 2

class PressButton {
private:
  byte pin;
  byte state;
  unsigned long lastMillis;
public:
  PressButton() {
    this->pin = PRESS_BUTTON_PIN;
    pinMode(this->pin, INPUT_PULLUP);
  }

  void attachIntr(void (*callback)()) {
    attachInterrupt(digitalPinToInterrupt(this->pin), callback, CHANGE);
  }

  void listen() {
    byte reading = digitalRead(this->pin);

    if (reading == LOW) {
      // longer press
      if ((millis() - this->lastMillis) > 3000) {
        this->state = reading;
      }
      // short press
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

extern PressButton;

#endif