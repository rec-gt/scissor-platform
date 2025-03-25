#include "Arduino.h"

class EncoderSwitch {
private:
  byte pin;
  byte state;
  byte lastState;
  unsigned long lastMillis;
public:
  EncoderSwitch(byte pin)
    : pin(pin) {
    pinMode(this->pin, INPUT_PULLUP);
  }

  void listen() {
    byte reading = digitalRead(this->pin);
    if (reading != this->lastState) {
      if ((millis() - this->lastMillis) > 50) {
        this->lastState = this->state = reading;
      }
    } else {
      this->lastMillis = millis();
    }
  }

  bool isOn() {
    return this->state == HIGH;
  }

  void print() {
    Serial.print(this->pin);
    Serial.print(", ");
    Serial.println(this->state);
  }
};