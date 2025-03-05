#include "Arduino.h"
#include "DisplayOLED.h"
#include "SystemEnums.h"

#ifndef downwardSensor_h
#define downwardSensor_h

class DownwardSensor {
private:
  byte pin;

  int tunningMinReading = 0;     // added when constructed, for tunning sensor, can be +ve/-ve number
  int installationHeight = 500;  // 需要在安裝後初始化/重新定義一次

  bool isLiftUp = false;
  
  // for measuring downward distance
  float measuredDistance;

  // for calibration
  float averageReading = 0;
  float averageCount = 0;


  float calculateDistance(float reading) {
    float minReading = this->tunningMinReading;
    float maxReading = 1023;
    float minSensor = 0;
    float maxSensor = 1750;

    return ((reading - maxReading) / (minReading - maxReading)) * (minSensor - maxSensor) + maxSensor;
  }

public:
  DownwardSensor(byte pin, int tunningMinReading)
    : pin(pin), tunningMinReading(tunningMinReading) {
    pinMode(this->pin, INPUT);
  }

  void listen() {
    int reading = analogRead(this->pin);
    this->measuredDistance = this->calculateDistance(reading);

    if (this->measuredDistance <= this->installationHeight + 100) {
      this->isLiftUp = false;
    }

    if (this->measuredDistance >= this->installationHeight + 200) {
      this->isLiftUp = true;
    }
  }

  bool isUp() {
    return this->isLiftUp;
  }


  float getDistance() {
    return this->measuredDistance;
  }

  void calibrate() {
    int reading = analogRead(this->pin);
    this->averageReading += reading;
    this->averageCount++;
    Serial.print("Min. Reading: ");
    Serial.println(this->averageReading / this->averageCount);
  }


  void print() {
    Serial.print(analogRead(this->pin));
    Serial.print(", ");
    Serial.println(this->calculateDistance(analogRead(this->pin)));
  }
};

extern DownwardSensor downwardSensor;

#endif
