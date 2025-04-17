#include "Arduino.h"
#include "DisplayOLED.h"
#include "SystemEnums.h"
#include "NBIoT.h"
#include "DetectSystem.h"

#ifndef sensorManager_h
#define sensorManager_h

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

  void listenAll() {
    for (int i = 0; i < this->num; i++) {
      this->laserSensors[i].listen();
    }
  }

  // === checker ===

  bool isOneDetected() {
    for (int i = 0; i < this->num; i++) {
      if (this->laserSensors[i].isDetected()) {
        this->showOneDetected(i);
        nbiot.quickSend(this->getSensors8Status(), this->getSensors2Status(), detectSystem.getStatus(), 0);
        return true;
      }
    }
    return false;
  }

  bool areAllEscaped() {
    bool allEscaped = true;

    // see if all are escaped
    for (int i = 0; i < this->num; i++) {
      this->laserSensors[i].setEscapeBuffer(true);
      if (this->laserSensors[i].isDetected()) {
        allEscaped = false;
      }
    }

    // if all are escaped, remove escape-buffer
    if (allEscaped) {
      for (int i = 0; i < this->num; i++) {
        this->laserSensors[i].setEscapeBuffer(false);
      }
    }

    return allEscaped;
  }

  bool areAllHealthy() {
    for (int i = 0; i < this->num; i++) {
      if (this->laserSensors[i].healthCheck() == false) {
        this->showOneUnhealthy(i);
        return false;
      }
    }
    return true;
  }

  // === Setter ===

  void setAllBaseThreshold(bool toggle) {
    for (int i = 0; i < this->num; i++) {
      this->laserSensors[i].setBaseThreshold(toggle);
    }
  }

  // === checker ===

  void showOneDetected(byte i) {
    char* charArr[] = {
      " ",
      utils.num2Char(i + 0),
      " 號感應器",
    };

    char* c = utils.concatCharN(charArr, 3);

    displayOLED.print("", c, "檢測到障礙物", 100 + i);
  }

  void showOneUnhealthy(byte i) {
    char* charArr[] = {
      " ",
      utils.num2Char(i),
      " 號感應器故障",
    };

    char* c = utils.concatCharN(charArr, 3);

    displayOLED.print("", c, "", 500);
  }

  // === getter ===

  float getMinDistance() {
    float minDistance = this->laserSensors[0].getDistance();
    for (int i = 0; i < this->num; i++) {
      float distance = this->laserSensors[i].getDistance();
      if (distance < minDistance) {
        minDistance = distance;
      }
    }
    return minDistance;
  }

  byte getSensors8Status() {
    byte res = 255;
    for (size_t i = 0; i < 8; i++) {
      if (this->laserSensors[i].isDetected()) {
        res &= ~(1 << i);
      }
    }
    return res;
  }

  // byte getSensors8Status() {
  //   byte result = 0;

  //   byte res = 255;

  //   for (int i = 7; i >= 0; i--) {
  //     if (this->laserSensors[i].isDetected()) {
  //       res -= pow(2, i);

  //       // === bitwise operation ===
  //       result |= 1 << i;
  //     }
  //   }

  //   return res;
  // }

  byte getSensors2Status() {
    byte res = 207;  // 11001111

    for (int i = 10; i >= 8; i--) {
      if (this->laserSensors[i].isDetected()) { res -= pow(2, (i - 8)); }
    }
    return res;
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

#endif