#include "Arduino.h"
#include "SystemEnums.h"

#ifndef trafficLight_h
#define trafficLight_h

class TrafficLight {
private:
  byte redPin;
  byte yellowPin;
  byte greenPin;

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
      digitalWrite(redPin, HIGH);
      digitalWrite(yellowPin, LOW);
      digitalWrite(greenPin, LOW);
    }
  }

  void yellow() {
    if (this->lastStatus != TRAFFIC_YELLOW) {
      this->lastStatus = TRAFFIC_YELLOW;
      digitalWrite(yellowPin, HIGH);
      digitalWrite(redPin, LOW);
      digitalWrite(greenPin, LOW);
    }
  }

  void green() {
    if (this->lastStatus != TRAFFIC_GREEN) {
      this->lastStatus = TRAFFIC_GREEN;
      digitalWrite(greenPin, HIGH);
      digitalWrite(redPin, LOW);
      digitalWrite(yellowPin, LOW);
    }
  }

  void redYellow() {
    if (this->lastStatus != TRAFFIC_RED_YELLOW) {
      this->lastStatus = TRAFFIC_RED_YELLOW;
      digitalWrite(redPin, HIGH);
      digitalWrite(yellowPin, HIGH);
      digitalWrite(greenPin, LOW);
    }
  }

  void greenYellow() {
    if (this->lastStatus != TRAFFIC_GREEN_YELLOW) {
      this->lastStatus = TRAFFIC_GREEN_YELLOW;
      digitalWrite(redPin, LOW);
      digitalWrite(yellowPin, HIGH);
      digitalWrite(greenPin, HIGH);
    }
  }

  void off() {  // because of 5v relay, LOW == connect, HIGH == cut
    if (this->lastStatus != TRAFFIC_OFF) {
      this->lastStatus = TRAFFIC_OFF;
      digitalWrite(redPin, LOW);
      digitalWrite(yellowPin, LOW);
      digitalWrite(greenPin, LOW);
    }
  }

  void listen(int distance) {
    unsigned int _delay = 500;
    if (distance <= 550) {  // enter the RED signal range
      if (millis() - this->lastMillis > _delay) {
        this->red();
      }
    } else if ((550 < distance && distance <= 800)) {  // enter the YELLOW signal range
      if (millis() - this->lastMillis > _delay) {
        this->yellow();
      }
    } else if (1200 < distance) {  // enter the GREEN signal range
      if (millis() - this->lastMillis > _delay) {
        this->green();
      }
    } else {
      this->lastMillis = millis();
    }
  }

  TrafficStatus getStatus() {
    return this->lastStatus;
  }
};

extern TrafficLight trafficLight;

#endif
