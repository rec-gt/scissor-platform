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

  unsigned long lastMillis = 0;
public:
  byte sensorStatusX8 = 0;
  byte sensorStatusX2 = 0;

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
        this->quickSend();
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

  void getSensorsStatus() {
    byte resX8 = 255;
    for (size_t i = 0; i < 8; i++) {
      if (this->laserSensors[i].isDetected()) {
        resX8 &= ~(1 << i);
      }
    }

    this->sensorStatusX8 = resX8;

    byte resX2 = 255;
    for (size_t i = 8; i < 2; i++) {
      if (this->laserSensors[i].isDetected()) {
        resX2 &= ~(1 << i);
      }
    }

    this->sensorStatusX2 = resX2;
  }

  // === NBIoT ===
  void publishStatus() {
    // Digital Input + Output = sensorsStatus
    // Analog Input[0] : 1 = RUNNING, ...
    // Analog Input[1] : 0 = not lifted up, 1 = lifted up

    if (millis() - this->lastMillis >= 10 * 1000) {  // send every 10s
      this->lastMillis = millis();
      this->quickSend();
    }
  }

  void quickSend() {
    this->getSensorsStatus();
    String callbackCMD = "AT+MQTTPUB=\"rgt/"
                         + String(nbiot.IMEI) + "/in\",1,0,0,0,\"{\"seq\":1,\"csq\":"
                         + String(nbiot.CSQ) + ",\"sw\":0,\"din\":"
                         + String(this->sensorStatusX8) + ",\"dout\":"
                         + String(this->sensorStatusX2) + ",\"ain\":["
                         + String(detectSystem.getStatus()) + ","
                         + 0 + ",0,0],\"aout\":[0,0,0,0]}\"";
    // nbiot.sendCMDFast(
    //   "AT+MQTTPUB=\"rgt/"
    //   + String(nbiot.IMEI) + "/in\",1,0,0,0,\"{\"seq\":1,\"csq\":"
    //   + String(nbiot.CSQ) + ",\"sw\":0,\"din\":"
    //   + String(this->sensorStatusX8) + ",\"dout\":"
    //   + String(this->sensorStatusX2) + ",\"ain\":["
    //   + String(detectSystem.getStatus()) + ","
    //   + 0 + ",0,0],\"aout\":[0,0,0,0]}\"");

    nbiot.sendCMDAsync("AT+CSQ", callbackCMD);
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