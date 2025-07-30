#include "Arduino.h"
#include "DisplayOLED.h"
#include "SystemEnums.h"
#include "NBIoT.h"
#include "LaserSensor.h"
// #include "DetectSystem.h"

#ifndef sensorManager_h
#define sensorManager_h

class LaserSensorManager {
private:
  LaserSensor laserSensors[20];
  size_t num;

  unsigned long lastMillis = 0;
public:
  byte sensorStatusX8 = 0;
  byte sensorStatusX4 = 0;
  byte problemSensor = 0;

  LaserSensorManager(LaserSensor sensors[], size_t num)
    : num(num) {
    for (size_t i = 0; i < this->num; i++) {
      this->laserSensors[i] = sensors[i];
    };
  }

  void listenAll() {
    for (int i = 0; i < this->num; i++) {
      this->laserSensors[i].listen();
    }
    // also listen to 10 sensors status
    this->listenSensorsStatus();
  }

  // === checker ===

  bool isOneDetected() {
    bool _oneDetected = false;

    for (int i = 0; i < this->num; i++) {
      if (this->laserSensors[i].isDetected()) {
        this->problemSensor = i;
        _oneDetected = true;
        break;
      }
    }

    return _oneDetected;
  }

  bool areAllEscaped() {
    bool _allEscaped = true;

    for (int i = 0; i < this->num; i++) {
      if (this->laserSensors[i].isDetected()) {
        _allEscaped = false;
        break;
      }
    }

    for (int i = 0; i < this->num; i++) {
      this->laserSensors[i].setEscapeBuffer(!_allEscaped);
    }

    return _allEscaped;
  }

  bool areAllHealthy() {
    bool _allHealthy = true;

    for (int i = 0; i < this->num; i++) {
      if (!this->laserSensors[i].isHealthy()) {
        this->problemSensor = i;
        _allHealthy = false;
        break;
      }
    }

    return _allHealthy;
  }

  // === Setter ===

  void setAllBaseThreshold(bool toggle) {
    for (int i = 0; i < this->num; i++) {
      this->laserSensors[i].setBaseThreshold(toggle);
    }
  }

  // === checker ===

  void showOneDetected() {
    int i = this->problemSensor;
    
    char* charArr[] = {
      " ",
      utils.num2Char(i + 0),
      " 號感應器",
    };

    char* c = utils.concatCharN(charArr, 3);

    displayOLED.print("", c, "檢測到障礙物", DISPLAY_SYS_DETECTED_0 + i);
  }

  void showOneUnhealthy(byte i) {
    char* charArr[] = {
      " ",
      utils.num2Char(i),
      " 號感應器故障",
    };

    char* c = utils.concatCharN(charArr, 3);

    displayOLED.print("", c, "", DISPLAY_SENSOR_UNHEALTHY);
  }

  // === getter ===

  float getMinDistance() {
    float minDistance = this->laserSensors[0].getDistance();
    for (size_t i = 0; i < this->num; i++) {
      float distance = this->laserSensors[i].getDistance();
      if (distance < minDistance) {
        minDistance = distance;
      }
    }
    return minDistance;
  }

  void listenSensorsStatus() {
    byte resX8 = 255;
    for (size_t i = 0; i < 8; i++) {
      if (this->laserSensors[i].isDetected()) {
        resX8 &= ~(1 << i);
      }
    }

    this->sensorStatusX8 = resX8;

    byte resX4 = 255;
    if (this->laserSensors[8].isDetected()) {
      resX4 &= ~(1 << 0);  // idk why, ask Viki System's Keith
    }
    if (this->laserSensors[9].isDetected()) {
      resX4 &= ~(1 << 1);
    }

    this->sensorStatusX4 = resX4;
  }

  // === debug ===

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
};

extern LaserSensorManager sensorManager;

#endif