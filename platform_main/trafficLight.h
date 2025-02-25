#include "Arduino.h"

class TrafficLight {
private:
  byte redPin;
  byte yellowPin;
  byte greenPin;

public:
  TrafficLight(byte redPin, byte yellowPin, byte greenPin)
    : redPin(redPin), yellowPin(yellowPin), greenPin(greenPin) {
    pinMode(redPin, OUTPUT);
    pinMode(yellowPin, OUTPUT);
    pinMode(greenPin, OUTPUT);
  }

  void red() {
    digitalWrite(redPin, HIGH);
  }

  void yellow() {
    digitalWrite(yellowPin, HIGH);
  }

  void green() {
    digitalWrite(greenPin, HIGH);
  }

  void off() {
    digitalWrite((redPin, LOW);
    digitalWrite((yellowPin, LOW);
    digitalWrite((greenPin, LOW);
  }
};