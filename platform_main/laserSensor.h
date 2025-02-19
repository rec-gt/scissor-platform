#include "Arduino.h"

class LaserSensor {
private:
  byte pin;

  // threshold and buffer
  int baseThreshold = 500;  // 500 (default) or 300
  int tunningBuffer = 0;    // added when constructed, for tunning each sensors, can be +ve/-ve number
  int dangerBuffer = 0;     // used when vehicle suddenly stop

  // measured distance
  int measuredDistance;

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
  LaserSensor() {}
  LaserSensor(byte pin, int tunningBuffer)
    : pin(pin), tunningBuffer(tunningBuffer) {
    pinMode(this->pin, INPUT);
  }

  void changeBaseThreshold(bool toggle) {  // true = 300, false = 500
    this->baseThreshold = toggle ? 300 : 500;
  }

  void setDangerBuffer(bool toggle) {
    this->dangerBuffer = toggle ? 100 : 0;  // true = 100, false = 0
  }

  void listen() {
    int reading = analogRead(this->pin);
    this->measuredDistance = this->calculateDistance(reading);

    int threshold = this->baseThreshold + this->tunningBuffer + this->dangerBuffer;

    bool measure = this->measuredDistance <= threshold;

    if (measure) {
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

  void print(byte nth) {
    Serial.print(nth);
    Serial.print(": ");
    Serial.println(this->measuredDistance);
  }
};


class LaserSensorManager {
private:
  LaserSensor laserSensors[10];
  size_t num;
public:
  LaserSensorManager(LaserSensor laserSensors[], size_t num)
    : num(num) {
    for (size_t i = 0; i < num; i++) {
      this->laserSensors[i] = laserSensors[i];
    };
  }

  bool isOneDetected() {
    for (int i = 0; i < this->num; i++) {
      this->laserSensors[i].listen();
      this->laserSensors[i].print(i);
      if (this->laserSensors[i].isDetected()) {
        return true;
      }
    }
    return false;
  }

  bool isAllEscaped() {
    bool escaped = true;

    for (int i = 0; i < this->num; i++) {
      this->laserSensors[i].setDangerBuffer(true);
      this->laserSensors[i].listen();
      this->laserSensors[i].print(i);
      if (this->laserSensors[i].isDetected()) {
        escaped = false;
      }
    }
    return escaped;
  }

  void changeBaseThreshold(bool toggle) {
    for (int i = 0; i < this->num; i++) {
      this->laserSensors[i].changeBaseThreshold(toggle);
    }
  }
};
