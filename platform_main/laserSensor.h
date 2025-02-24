#include "Arduino.h"
#include "displayOLED.h"

class LaserSensor {
private:
  byte pin;

  // threshold and buffer
  int baseThreshold = 800;  // 800 (default) or 500
  int tunningBuffer = 0;    // added when constructed, for tunning each sensors, can be +ve/-ve number
  int dangerBuffer = 0;     // used when vehicle suddenly stop

  // measured distance
  float measuredDistance;

  // for debounce
  unsigned long lastMillis;
  bool detected = false;

  float calculateDistance(float reading) {
    float min_reading = 220;
    float max_reading = 1023;
    float min_sensor = 0;
    float max_sensor = 1750;

    return ((reading - max_reading) / (min_reading - max_reading)) * (min_sensor - max_sensor) + max_sensor;
  }

public:
  LaserSensor() {}

  LaserSensor(byte pin, int tunningBuffer)
    : pin(pin), tunningBuffer(tunningBuffer) {
    pinMode(this->pin, INPUT);
  }

  void changeBaseThreshold(bool toggle) {  // true = 500, false = 800
    this->baseThreshold = toggle ? 500 : 800;
  }

  void setDangerBuffer(bool toggle) {
    this->dangerBuffer = toggle ? 200 : 0;  // true = 100, false = 0
  }

  void listen() {
    int reading = analogRead(this->pin);

    this->measuredDistance = this->calculateDistance(reading);

    int threshold = this->baseThreshold + this->tunningBuffer + this->dangerBuffer;


    Serial.print(this->pin);
    Serial.print(": ");
    Serial.print(reading);
    Serial.print(", ");
    Serial.print(this->measuredDistance);
    Serial.print(", ");
    Serial.println(threshold);


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

  bool isDetected() {
    return this->detected;
  }

  bool healthCheck() {
    if (analogRead(this->pin) < 50) {  // normal sensor reading should be 200+, if sensor fails, reading drops to ~0
      return false;
    }
    return true;
  }

  void print(byte nth) {
    Serial.print("Threshold: ");
    Serial.print(this->baseThreshold);
    Serial.print(", ");
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
      if (this->laserSensors[i].isDetected()) {
        return true;
      }
    }
    return false;
  }

  bool areAllEscaped() {
    bool allEscaped = true;

    for (int i = 0; i < this->num; i++) {
      this->laserSensors[i].setDangerBuffer(true);
      this->laserSensors[i].listen();
      if (this->laserSensors[i].isDetected()) {
        allEscaped = false;
      }
    }

    if (allEscaped) {
      for (int i = 0; i < this->num; i++) {
        this->laserSensors[i].setDangerBuffer(false);
      }
    }

    return allEscaped;
  }

  void changeBaseThreshold(bool toggle) {
    for (int i = 0; i < this->num; i++) {
      this->laserSensors[i].changeBaseThreshold(toggle);
    }
  }

  bool areAllHealthy(DisplayOLED displayOLED) {
    for (int i = 0; i < this->num; i++) {
      if (this->laserSensors[i].healthCheck() == false) {

        char* c1 = displayOLED.concatChar("感應器 ", displayOLED.num2Char(i + 1));
        char* c2 = displayOLED.concatChar(c1, " 故障");
        displayOLED.print("", c2, "", 404);

        return false;
      }
    }
    return true;
  }
};
