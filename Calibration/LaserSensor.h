#include "Arduino.h"

class LaserSensor {
private:
  byte pin;

  int tunningMinReading = 0;
  float measuredDistance = 0;
  float averageReading = 0;
  float averageCount = 0;

  float calculateDistance(float reading) {
    float minReading = this->tunningMinReading;
    float maxReading = 1023;
    float minDistance = 0;
    float maxDistance = 1750;

    return ((reading - maxReading) / (minReading - maxReading)) * (minDistance - maxDistance) + maxDistance;
  }

public:
  LaserSensor() {}

  LaserSensor(byte pin, int tunningMinReading)
    : pin(pin), tunningMinReading(tunningMinReading) {
    pinMode(this->pin, INPUT);
  }

  void voidReading() {
    analogRead(this->pin);
  }

  float calibrate() {
    this->averageReading += analogRead(this->pin);
    this->averageCount++;
    return this->averageReading / this->averageCount;
  }
};


class LaserSensorManager {
private:
  LaserSensor laserSensors[20];
  size_t num;
public:
  LaserSensorManager(LaserSensor laserSensors[], size_t num)
    : num(num) {
    for (size_t i = 0; i < num; i++) {
      this->laserSensors[i] = laserSensors[i];
    };
  }

  void calibrateAll() {
    for (int i = 0; i < 300; i++) {
      if (i < 50) {
        Serial.println("Reject first 50 sampling");
        for (size_t j = 0; j < this->num; j++) {
          this->laserSensors[j].voidReading();
        }
        continue;  // reject first 50 reading
      } else {
        Serial.print("Calibrate all, ");
        Serial.println(i);
        for (size_t j = 0; j < this->num; j++) {
          Serial.print(j);
          Serial.print(", ");
          Serial.println(this->laserSensors[j].calibrate());
        }
        delay(10);
      }
    }
  }
};
