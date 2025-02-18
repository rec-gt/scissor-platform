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
    digitalWrite(pin, HIGH);
  }

  void normalListen() {
    state = digitalRead(pin);
  }

  void debounceListen() {
    byte reading = digitalRead(pin);

    if (reading == LOW) {
      if ((millis() - lastMillis) > 150) {
        state = reading;
      }
    } else {
      state = HIGH;
      lastMillis = millis();
    }
  }

  bool isPressed() {
    return state == LOW;
  }
};