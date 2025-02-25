#include "Arduino.h"

class TrafficLight {
private:
  byte redPin;
  byte yellowPin;
  byte greenPin;

  enum TrafficStatus { NONE,
                       RED,
                       YELLOW,
                       GREEN };

  TrafficStatus status = NONE;
  TrafficStatus lastStatus = NONE;

public:
  TrafficLight(byte redPin, byte yellowPin, byte greenPin)
    : redPin(redPin), yellowPin(yellowPin), greenPin(greenPin) {
    pinMode(redPin, OUTPUT);
    pinMode(yellowPin, OUTPUT);
    pinMode(greenPin, OUTPUT);
    this->off();
  }

  void red() {
    if (this->lastStatus != RED) {
      this->lastStatus = RED;
      digitalWrite(redPin, LOW);
    }
  }

  void yellow() {
    if (this->lastStatus != YELLOW) {
      this->lastStatus = YELLOW;
      digitalWrite(yellowPin, LOW);
    }
  }

  void green() {
    if (this->lastStatus != GREEN) {
      this->lastStatus = GREEN;
      digitalWrite(greenPin, LOW);
    }
  }

  void off() {
    digitalWrite(redPin, HIGH);
    digitalWrite(yellowPin, HIGH);
    digitalWrite(greenPin, HIGH);
    this->lastStatus = NONE;
  }

  void listen(int distance) {
    Serial.println(distance);
    if (distance < 500) {
      this->red();
    } else if (distance < 800) {
      this->yellow();
    } else if (distance < 1200) {
      this->green();
    } else {
      this->off();
    }
  }
};