#include "Arduino.h"
#include "SystemEnums.h"

class TrafficLight {
private:
  byte redPin;
  byte yellowPin;
  byte greenPin;

  TrafficStatus status = TRAFFIC_OFF;
  TrafficStatus lastStatus = TRAFFIC_OFF;

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
    digitalWrite(redPin, LOW);
    digitalWrite(yellowPin, HIGH);
    digitalWrite(greenPin, HIGH);
  }

  void yellow() {
    digitalWrite(yellowPin, LOW);
    digitalWrite(redPin, HIGH);
    digitalWrite(greenPin, HIGH);
  }

  void green() {
    digitalWrite(greenPin, LOW);
    digitalWrite(redPin, HIGH);
    digitalWrite(yellowPin, HIGH);
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
    if (distance <= 500 && this->lastStatus != TRAFFIC_RED) {  // enter the RED signal range
      if (millis() - this->lastMillis > 500) {
        this->lastStatus = TRAFFIC_RED;  // change to RED status
        this->red();
      }
    } else if ((500 < distance && distance <= 800) && this->lastStatus != TRAFFIC_YELLOW) {  // enter the YELLOW signal range
      if (millis() - this->lastMillis > 500) {
        this->lastStatus = TRAFFIC_YELLOW;  // change to YELLOW status
        this->yellow();
      }
    } else if (800 < distance && this->lastStatus != TRAFFIC_GREEN) {  // enter the GREEN signal range
      if (millis() - this->lastMillis > 500) {
        this->lastStatus = TRAFFIC_GREEN;  // change to GREEN status
        this->green();
      }
    } else {
      this->lastMillis = millis();
    }
  }
};