#include "Arduino.h"
#include "DisplayOLED.h"
#include "SystemEnums.h"

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
  float measuredDistance;

  // for debounce
  unsigned long lastMillis;
  bool detected = false;

  // for calibration
  float averageReading = 0;
  float averageCount = 0;

  // for detected senson
  byte detectedNum;

  float calculateDistance(float reading) {
    float minReading = this->tunningMinReading;
    float maxReading = 1023;
    float minSensor = 0;
    float maxSensor = 1750;

    return ((reading - maxReading) / (minReading - maxReading)) * (minSensor - maxSensor) + maxSensor;
  }

public:
  LaserSensor() {}

  LaserSensor(byte pin, int longerThreshold, int shorterThreshold, int tunningMinReading)
    : pin(pin), longerThreshold(longerThreshold), shorterThreshold(shorterThreshold), tunningMinReading(tunningMinReading) {
    pinMode(this->pin, INPUT);
  }

  void listen() {
    int reading = analogRead(this->pin);

    this->measuredDistance = this->calculateDistance(reading);

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

  void changeBaseThreshold(bool toggle) {
    this->baseThreshold = toggle ? this->shorterThreshold : this->longerThreshold;
  }

  void setEscapeBuffer(bool toggle) {
    this->escapeBuffer = toggle ? CONST_ESCAPE_BUFFER : 0;
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

  bool isOneDetected() {
    for (int i = 0; i < this->num; i++) {
      this->laserSensors[i].listen();
      if (this->laserSensors[i].isDetected()) {
        this->showOneDetected(i);
        return true;
      }
    }
    return false;
  }

  void showOneDetected(byte i) {
    char* orientation;
    if (i <= 1) {
      orientation = "前方 ";
    } else if (i <= 7) {
      orientation = "上方 ";
    } else if (i <= 9) {
      orientation = "後方 ";
    }

    char* charArr[] = {
      orientation,
      utils.num2Char(i + 0),
      " 號感應器",
    };

    char* c = utils.concatCharN(charArr, 3);

    displayOLED.print(c, "檢測到障礙物", "", 100 + i);
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

  void changeAllBaseThreshold(bool toggle) {
    for (int i = 0; i < this->num; i++) {
      this->laserSensors[i].changeBaseThreshold(toggle);
    }
  }

  bool areAllHealthy() {
    for (int i = 0; i < this->num; i++) {
      if (this->laserSensors[i].healthCheck() == false) {

        char* charArr[] = { " ", utils.num2Char(i), " 號感應器故障" };
        char* c = utils.concatCharN(charArr, sizeof(charArr) / sizeof(charArr[0]));
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

  void print(byte i) {
    Serial.print(i);
    Serial.print(", ");
    Serial.print(this->laserSensors[i].getDistance());
    Serial.print(", ");
    Serial.println(this->laserSensors[i].isDetected());
  }

  void printAll() {
    for (int i = 0; i < this->num; i++) {
      this->laserSensors[i].listen();
      float distance = this->laserSensors[i].getDistance();
      Serial.print(i);
      Serial.print(", ");
      Serial.println(distance);
    }
  }

  byte getSensors8Status() {
    byte res = 255;

    for (int i = 7; i >= 0; i--) {
      if (this->laserSensors[i].isDetected()) { res -= pow(2, i); }
    }

    return res;
  }

  byte getSensors2Status() {
    byte res = 207;  // 11001111

    for (int i = 10; i >= 8; i--) {
      if (this->laserSensors[i].isDetected()) { res -= pow(2, (i - 8)); }
    }
    return res;
  }
};
