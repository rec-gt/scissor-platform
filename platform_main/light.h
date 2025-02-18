#include "Arduino.h"

class Light {
private:
  byte pin;
  byte lastState;
  unsigned long lastMillis;

public:
  Light(byte pin)
    : pin(pin), lastState(LOW), lastMillis(millis()) {
    pinMode(pin, OUTPUT);
  }

  void on() {
    digitalWrite(pin, HIGH);
  }

  void off() {
    digitalWrite(pin, LOW);
  }

  void bling() {
    unsigned long currMillis = millis();
    byte currState = digitalRead(pin);
    if (currMillis - lastMillis > 150) {
      lastMillis = currMillis;

      if (currState == lastState) {
        lastState = !currState;
        digitalWrite(pin, lastState);
      }
    }
  }
};