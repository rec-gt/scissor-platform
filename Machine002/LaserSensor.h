#include "Arduino.h"
#include "DisplayOLED.h"
#include "SystemEnums.h"

#ifndef laserSensor_h
#define laserSensor_h

class LaserSensor {
private:
  byte pin;

  // threshold and buffer
  int baseThreshold = CONST_LONGER_THRESHOLED;
  int longerThreshold = CONST_LONGER_THRESHOLED;
  int shorterThreshold = CONST_SHORTER_THRESHOLED;
  int tunningMinReading = 0;  // added when constructed, for tunning each sensors, can be +ve/-ve number
  int escapeBuffer = 0;       // used when vehicle suddenly stop

  // measured distance
  float reading;
  float measuredDistance;

  // for debounce
  unsigned long lastMillis;
  bool detected = false;

public:
  LaserSensor() {}

  LaserSensor(byte pin, int longerThreshold, int shorterThreshold, int tunningMinReading)
    : pin(pin), longerThreshold(longerThreshold), shorterThreshold(shorterThreshold), tunningMinReading(tunningMinReading) {
    pinMode(this->pin, INPUT);
  }

  void listen() {
    // this->reading = this->avgRead();
    this->reading = analogRead(this->pin);

    this->measuredDistance = this->calcDistance();

    int threshold = this->baseThreshold + this->escapeBuffer;

    bool measurement = this->measuredDistance <= threshold;

    if (measurement) {
      if ((millis() - this->lastMillis) > 750) {
        this->detected = true;
      }
    } else {
      this->detected = false;
      this->lastMillis = millis();
    }
  }

  float avgRead() {
    unsigned long avg = 0;
    for (size_t i = 0; i < 32; i++) {
      avg += analogRead(this->pin);
    };
    return avg / 32.;
  }

  float calcDistance() {
    float pinReading = this->reading;
    float minReading = this->tunningMinReading;
    float maxReading = 1023.;
    float minSensor = 0.;
    float maxSensor = 1750.;

    return ((pinReading - maxReading) / (minReading - maxReading)) * (minSensor - maxSensor) + maxSensor;
  }

  // === setter ===

  void setBaseThreshold(bool toggle) {
    this->baseThreshold = toggle ? this->shorterThreshold : this->longerThreshold;
  }

  void setEscapeBuffer(bool toggle) {
    this->escapeBuffer = toggle ? CONST_ESCAPE_BUFFER : 0;
  }

  // === getter ===

  bool isDetected() {
    return this->detected;
  }

  float getReading() {
    return this->reading;
  }

  float getDistance() {
    return this->measuredDistance;
  }

  // === utils ===

  bool healthCheck() {
    return analogRead(this->pin) > 50;  // normal sensor reading should be 200+, if sensor fails, reading drops to ~0
  }

  ~LaserSensor(){};
};

#endif
