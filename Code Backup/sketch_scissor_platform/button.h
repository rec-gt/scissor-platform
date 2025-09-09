#include "Arduino.h"

class Button {
private:
  byte pin;
  byte state;
  byte lastReading;
  unsigned long lastDebounceTime = 0;
  unsigned long debounceDelay = 50;
public:
  Button(byte pin) {
    this->pin = pin;
    lastReading = LOW;
    init();
  }

  void init() {
    pinMode(pin, INPUT);
    update();
  }

  void update() {
    byte newReading = digitalRead(pin);

    if (newReading != lastReading) {
      lastDebounceTime = millis();
    }

    if (millis() - lastDebounceTime > debounceDelay) {
      state = newReading;
    }

    lastReading = newReading;
  }

  byte getState() {
    update();
    return state;
  }

  bool isPressed() {
    return (getState() == HIGH);
  }
};