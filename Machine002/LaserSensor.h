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
    // analogReference(EXTERNAL);
    analogReference(DEFAULT);
  }

  void listen() {
    this->reading = this->avgRead();

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


class LaserSensorManager {
private:
  LaserSensor laserSensors[20];
  size_t num;
public:
  LaserSensorManager(LaserSensor sensors[], size_t num)
    : num(num) {
    for (size_t i = 0; i < this->num; i++) {
      this->laserSensors[i] = sensors[i];
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
      this->laserSensors[i].setBaseThreshold(toggle);
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
    float minDistance = this->laserSensors[0].getDistance();
    for (int i = 0; i < this->num; i++) {
      float distance = this->laserSensors[i].getDistance();
      if (distance < minDistance) {
        minDistance = distance;
      }
    }
    return minDistance;
  }

  void printOne(byte i) {
    Serial.print("Sensor: ");
    Serial.print(i);
    Serial.print(", Reading: ");
    Serial.print(this->laserSensors[i].getReading());
    Serial.print(", Distance: ");
    Serial.print(this->laserSensors[i].getDistance());
    Serial.println();
  }

  void printAll() {
    for (int i = 0; i < this->num; i++) {
      this->printOne(i);
    }
  }

  byte getSensors8Status() {
    byte result = 0;

    byte res = 255;

    for (int i = 7; i >= 0; i--) {
      if (this->laserSensors[i].isDetected()) {
        res -= pow(2, i);

        // === bitwise operation ===
        result |= 1 << i;
      }
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
