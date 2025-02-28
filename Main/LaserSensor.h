#include "Arduino.h"
#include "DisplayOLED.h"
#include "SystemEnums.h"

class LaserSensor {
private:
  byte pin;

  // threshold and buffer
  int baseThreshold = CONST_LONGER_THRESHOLED;
  int tunningMinReading = 0;  // added when constructed, for tunning each sensors, can be +ve/-ve number
  int escapeBuffer = 0;       // used when vehicle suddenly stop

  // measured distance
  float measuredDistance;

  // for debounce
  unsigned long lastMillis;
  bool detected = false;

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
  LaserSensor() {}

  LaserSensor(byte pin, int tunningMinReading)
    : pin(pin), tunningMinReading(tunningMinReading) {
    pinMode(this->pin, INPUT);
  }

  void changeBaseThreshold(bool toggle) {
    this->baseThreshold = toggle ? CONST_SHORTER_THRESHOLED : CONST_LONGER_THRESHOLED;
  }

  void setEscapeBuffer(bool toggle) {
    this->escapeBuffer = toggle ? CONST_ESCAPE_BUFFER : 0;
  }

  void listen() {
    int reading = analogRead(this->pin);

    this->measuredDistance = this->calculateDistance(reading);

    int threshold = this->baseThreshold + this->escapeBuffer;

    bool measure = this->measuredDistance <= threshold;

    if (measure) {
      if ((millis() - this->lastMillis) > 750) {
        this->detected = true;
      }
    } else {
      this->detected = false;
      this->lastMillis = millis();
    }
  }

  float getDistance() {
    return this->measuredDistance;
  }

  bool isDetected() {
    return this->detected;
  }

  bool healthCheck() {
    if (analogRead(this->pin) < 50) {  // normal sensor reading should be 200+, if sensor fails, reading drops to ~0
      return false;
    }
    return true;
  }

  void calibrate() {
    int reading = analogRead(this->pin);
    this->averageReading += reading;
    this->averageCount++;
    Serial.print("Min. Reading: ");
    Serial.println(this->averageReading / this->averageCount);
  }

  void print(byte nth) {
    Serial.print(analogRead(this->pin));
    Serial.print(", ");
    Serial.println(this->calculateDistance(analogRead(this->pin)));
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
      if (this->laserSensors[i].isDetected()) {
        return true;
      }
    }
    return false;
  }

  bool areAllEscaped() {
    bool allEscaped = true;

    for (int i = 0; i < this->num; i++) {
      this->laserSensors[i].setEscapeBuffer(true);
      this->laserSensors[i].listen();
      if (this->laserSensors[i].isDetected()) {
        allEscaped = false;
      }
    }

    if (allEscaped) {
      for (int i = 0; i < this->num; i++) {
        this->laserSensors[i].setEscapeBuffer(false);
      }
    }

    return allEscaped;
  }

  void changeBaseThreshold(bool toggle) {
    for (int i = 0; i < this->num; i++) {
      this->laserSensors[i].changeBaseThreshold(toggle);
    }
  }

  bool areAllHealthy() {
    for (int i = 0; i < this->num; i++) {
      if (this->laserSensors[i].healthCheck() == false) {

        char* cArr[] = { "感應器 ", utils.num2Char(i + 1), " 故障" };
        char* c = utils.concatCharN(cArr, 3);
        displayOLED.print("", c, "", 500);

        return false;
      }
    }
    return true;
  }

  int getMinDistance() {
    int minDistance = this->laserSensors[0].getDistance();
    for (int i = 0; i < this->num; i++) {
      float distance = this->laserSensors[i].getDistance();
      if (distance < minDistance) {
        minDistance = distance;
      }
    }
    return minDistance;
  }
};
