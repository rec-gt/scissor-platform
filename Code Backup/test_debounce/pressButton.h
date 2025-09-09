#include "Arduino.h"

class PressButton {
private:
  byte pin;
  byte state;
  byte lastState;
  unsigned long lastMillis;
public:
  PressButton(byte pin)
    : pin(pin) {
    pinMode(this->pin, INPUT);
  }

  void listen() {
    this->state = digitalRead(this->pin);
  }

  void debounceListen() {
    byte reading = digitalRead(this->pin);

    if (reading == HIGH) {
      if ((millis() - this->lastMillis) > 150) {
        this->state = reading;
      }
    } else {
      this->state = LOW;
      this->lastMillis = millis();
    }

    // Serial.print(reading);
    // Serial.print(", ");
    // Serial.print(this->state);
    // Serial.print(", ");
    // Serial.print(millis());
    // Serial.println();
  }

  bool isPressed() {
    return this->state == HIGH;  // once pressed (i.e., GND), the pin become 0V
  }

  float print() {
  }
};