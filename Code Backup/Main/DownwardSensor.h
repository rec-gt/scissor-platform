#include "Arduino.h"
#include "DisplayOLED.h"
#include "SystemEnums.h"

#ifndef downwardSensor_h
#define downwardSensor_h

class DownwardSensor {
private:
  byte pin;

  int tunningMinReading = 0;     // added when constructed, for tunning sensor, can be +ve/-ve number
  int installationHeight = 717;  // 需要在安裝後初始化/重新定義一次

  bool isLiftUp = false;

  // for measuring downward distance
  float measuredDistance;

  // for calibration
  float totalCalReading = 0;
  float totalCalDistance = 0;
  float totalCalCount = 0;


  float calculateDistance(float reading) {
    float minReading = this->tunningMinReading;
    float maxReading = 816;
    float minSensor = 0;
    float maxSensor = 3000;

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

    if (this->measuredDistance >= this->installationHeight + 300) {
      this->isLiftUp = true;
    }
  }

  bool isUp() {
    return this->isLiftUp;
  }

  int getReading() {
    return analogRead(this->pin);
  }

  float getDistance() {
    return this->measuredDistance;
  }

  void calibrateReading() {
    int reading = analogRead(this->pin);
    this->totalCalReading += reading;
    this->totalCalCount++;
    Serial.print("Min. Reading: ");
    Serial.println(this->totalCalReading / this->totalCalCount);
  }

  void calibrateDistance() {
    int reading = analogRead(this->pin);
    this->totalCalDistance += this->calculateDistance(reading);
    this->totalCalCount++;
    Serial.print("Cal. Distance: ");
    Serial.println(this->totalCalDistance / this->totalCalCount);
  }


  void print() {
    int reading = analogRead(this->pin);
    Serial.print(reading);
    Serial.print(", ");
    Serial.println(this->calculateDistance(reading));
  }
};

extern DownwardSensor;

#endif
