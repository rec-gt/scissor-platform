#include "Arduino.h"

class LaserSensor {
private:
  byte pin;

  // threshold and buffer
  float baseThreshold = 500;  // 500 (default) or 300
  float tunningBuffer = 0;    // added when constructed, for tunning each sensors, can be +ve/-ve number
  float dangerBuffer = 0;     // used when vehicle suddenly stop

  // measured distance
  float measuredDistance;

  // for debounce
  unsigned long lastMillis;
  bool detected = false;

  float calculateDistance(float reading) {
    float min_factor = 192;
    float max_factor = 965;
    float min_sensor = 0;
    float max_sensor = 2000;

    return ((reading - max_factor) / (min_factor - max_factor)) * (min_sensor - max_sensor) + max_sensor;
  }

public:
  LaserSensor(byte pin, float tunningBuffer)
    : pin(pin), tunningBuffer(tunningBuffer) {
    pinMode(this->pin, INPUT);
  }

  void changeBaseThreshold(bool toggle) {
    this->baseThreshold = toggle ? 300 : 500;
  }

  void setDangerBuffer(bool toggle) {
    this->dangerBuffer = toggle ? 100 : 0;  // true = 100, false = 0
  }

  void debounceListen() {
    float reading = analogRead(this->pin);
    this->measuredDistance = this->calculateDistance(reading);
    float threshold = this->baseThreshold + this->tunningBuffer + this->dangerBuffer;

    bool measure = this->measuredDistance <= threshold;

    if (measure == true) {
      if ((millis() - this->lastMillis) > 1000) {
        this->detected = true;
      }
    } else {
      this->detected = false;
      this->lastMillis = millis();
    }
  }

  bool isDetected() {
    return this->detected;
  }

  LaserSensor& print() {
    return *this;
  }

  void byVoltage() {
    Serial.println(this->measuredDistance / 1023 * 5);
  }

  void byValue() {
    Serial.println(this->measuredDistance);
  }

  float getDistance() {
    return this->measuredDistance;
  }
};
