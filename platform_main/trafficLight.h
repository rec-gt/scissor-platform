#include "Arduino.h"


// because of 5v relay, LOW == connect, HIGH == cut
class TrafficLight {
private:
  byte redPin;
  byte yellowPin;
  byte greenPin;

  enum TrafficStatus {
    RED,
    YELLOW,
    GREEN
  };

  TrafficStatus status = GREEN;
  TrafficStatus lastStatus = GREEN;

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

  void off() {
    digitalWrite(redPin, HIGH);
    digitalWrite(yellowPin, HIGH);
    digitalWrite(greenPin, HIGH);
  }

  void listen(int distance) {

    if (distance <= 500 && this->lastStatus != RED) {  // enter the RED signal range
      if (millis() - this->lastMillis > 500) {
        this->lastStatus = RED;  // change to RED status
        this->red();
      }
    } else if ((500 < distance && distance <= 800) && this->lastStatus != YELLOW) {  // enter the YELLOW signal range
      if (millis() - this->lastMillis > 500) {
        this->lastStatus = YELLOW;  // change to YELLOW status
        this->yellow();
      }
    } else if (800 < distance && this->lastStatus != GREEN) {  // enter the GREEN signal range
      if (millis() - this->lastMillis > 500) {
        this->lastStatus = GREEN;  // change to GREEN status
        this->green();
      }
    } else {
      this->lastMillis = millis();
    }
  }
};