#include "Arduino.h"
#include "SystemEnums.h"

class TrafficLight {
private:
  byte redPin;
  byte yellowPin;
  byte greenPin;

  TrafficStatus status = TRAFFIC_GREEN;
  TrafficStatus lastStatus = TRAFFIC_GREEN;

  unsigned long lastMillis;

public:
  TrafficLight(byte redPin, byte yellowPin, byte greenPin)
    : redPin(redPin), yellowPin(yellowPin), greenPin(greenPin) {
    pinMode(redPin, OUTPUT);
    pinMode(yellowPin, OUTPUT);
    pinMode(greenPin, OUTPUT);
    this->off();
  }

  void red() {
    if (this->lastStatus != TRAFFIC_RED) {
      this->lastStatus = TRAFFIC_RED;
      digitalWrite(redPin, LOW);
      digitalWrite(yellowPin, HIGH);
      digitalWrite(greenPin, HIGH);
    }
  }

  void yellow() {
    if (this->lastStatus != TRAFFIC_YELLOW) {
      this->lastStatus = TRAFFIC_YELLOW;
      digitalWrite(yellowPin, LOW);
      digitalWrite(redPin, HIGH);
      digitalWrite(greenPin, HIGH);
    }
  }

  void green() {
    if (this->lastStatus != TRAFFIC_GREEN) {
      this->lastStatus = TRAFFIC_GREEN;
      digitalWrite(greenPin, LOW);
      digitalWrite(redPin, HIGH);
      digitalWrite(yellowPin, HIGH);
    }
  }

  void off() {  // because of 5v relay, LOW == connect, HIGH == cut
    if (this->lastStatus != TRAFFIC_OFF) {
      this->lastStatus = TRAFFIC_OFF;
      digitalWrite(redPin, HIGH);
      digitalWrite(yellowPin, HIGH);
      digitalWrite(greenPin, HIGH);
    }
  }

  void listen(int distance) {
    if (distance <= 500) {  // enter the RED signal range
      if (millis() - this->lastMillis > 500) {
        this->red();
      }
    } else if ((500 < distance && distance <= 800)) {  // enter the YELLOW signal range
      if (millis() - this->lastMillis > 500) {
        this->yellow();
      }
    } else if (800 < distance) {  // enter the GREEN signal range
      if (millis() - this->lastMillis > 500) {
        this->green();
      }
    } else {
      this->lastMillis = millis();
    }
  }
};