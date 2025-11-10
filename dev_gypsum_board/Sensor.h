#include "U8g2lib.h"

#ifndef sensor_h
#define sensor_h


class Sensor {
private:
  byte pin;

  uint16_t avgRead() {
    uint16_t avg = 0;
    for (size_t i = 0; i < 64; i++) {
      avg += analogRead(this->pin);
    };
    return avg / 64;
  }

public:
  uint16_t reading = 0;

  Sensor(byte pin)
    : pin(pin) {
    pinMode(this->pin, INPUT);
  }

  void loop() {
    this->reading = this->avgRead();
  }

  ~Sensor(){};
};

#endif
