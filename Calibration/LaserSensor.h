#include "Arduino.h"

class LaserSensor {
private:
  byte pin;

  float averageReading = 0;
  int averageReadingCount = 0;

  int tunningMinReading = 0;
  float averageDistance = 0;
  int averageDistanceCount = 0;

public:
  LaserSensor() {}

  LaserSensor(byte pin, int tunningMinReading)
    : pin(pin), tunningMinReading(tunningMinReading) {
    pinMode(this->pin, INPUT);
  }

  int getReading() {
    return analogRead(this->pin);
  }

  float calibrateReading() {
    this->averageReading += analogRead(this->pin);
    this->averageReadingCount++;
    return this->averageReading / this->averageReadingCount;
  }

  float calibrateDistance(float reading) {
    float minReading = this->tunningMinReading;
    float maxReading = 1023;
    float minDistance = 0;
    float maxDistance = 1750;

    return ((reading - maxReading) / (minReading - maxReading)) * (minDistance - maxDistance) + maxDistance;
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

  void calibrateAllReading() {
    for (int i = 0; i < 300; i++) {
      if (i < 50) {
        Serial.println("Reject first 50 sampling");
        for (size_t j = 0; j < this->num; j++) {
          this->laserSensors[j].getReading();
        }
        continue;  // reject first 50 reading
      } else {
        Serial.print("Iteration: ");
        Serial.println(i);
        for (size_t j = 0; j < this->num; j++) {
          Serial.print(j);
          Serial.print(", ");
          Serial.println(this->laserSensors[j].calibrateReading());
        }
        delay(10);
      }
    }
  }

  void calibrateAllDistance() {
    for (int i = 0; i < 100; i++) {
      if (i < 10) {
        Serial.println("Reject first 10 sampling");
        for (size_t j = 0; j < this->num; j++) {
          this->laserSensors[j].getReading();
        }
        continue;  // reject first 10 reading
      } else {
        Serial.print("Iteration: ");
        Serial.println(i);
        for (size_t j = 0; j < this->num; j++) {
          Serial.print(j);
          Serial.print(", ");
          Serial.println(this->laserSensors[j].calibrateDistance(this->laserSensors[j].getReading()));
        }
        delay(10);
      }
    }
  }
};
