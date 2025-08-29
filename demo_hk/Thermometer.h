#include "Arduino.h"

class Thermometer {
private:
  byte pin;
  int temperature = 2500;  //25
  int queue[10] = {};

  void queueAppend(int value) {
    for (size_t i = 1; i < 10; i++) {
      this->queue[i - 1] = this->queue[i];
    }
    this->queue[9] = value;
  }

  int getAvg() {
    uint16_t sum = 0;
    for (size_t i = 0; i < 10; i++) {
      sum += this->queue[i];
    }
    return sum / 10;
  }

public:
  Thermometer(byte pin)
    : pin(pin) {
    pinMode(pin, INPUT);
  }

  void listen() {
    unsigned long avg = 0;
    for (size_t i = 0; i < 64; i++) {
      avg += analogRead(this->pin);
    };

    this->temperature = map((avg / 64.), 204.6, 1023, 40, 20000);
    this->queueAppend(this->temperature);
  }

  int get() {
    return this->getAvg();
    // return this->temperature;
  }
};