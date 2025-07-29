#include "Arduino.h"
#include "SystemEnums.h"
#include "DisplayOLED.h"
#include "SensorManager.h"
#include "TrafficLight.h"
#include "NBIoT.h"
#include "Globals.h"

#ifndef detectSystem_h
#define detectSystem_h

class DetectSystem {
private:
  SystemStatus status = SYS_INIT;
  SystemStatus lastStatus = SYS_INIT;
  unsigned long prevMillis = 0;

public:
  SystemStatus getStatus() {
    return this->status;
  }

  void set(SystemStatus status) {
    if (status == this->lastStatus) {
      return;
    }

    if (status == SYS_RUNNING) {
      displayOLED.print("", "系統運作中", "0120120120", DISPLAY_SYS_RUNNING);
    }

    this->lastStatus = status;
    this->status = status;

    Serial.println(STATUS_STR[status]);
  }

  bool is(SystemStatus targetStatus) {
    return this->status == targetStatus;
  }

  void setPublishMsg() {
    String content = "{\"csq\":";
    content += nbiot.CSQ;
    content += ",";
    content += "\"cgatt\":";
    content += nbiot.CGATT;
    content += ",";
    content += "\"cereg\":\"";
    content += nbiot.CEREG;
    content += "\"";
    content += ",";
    content += "\"din\":";
    content += String(sensorManager.sensorStatusX8);
    content += ",";
    content += "\"dout\":";
    content += String(sensorManager.sensorStatusX4);
    content += "}";

    int contentLen = content.length();
    String cmd = "AT+QMTPUB=0,0,0,0,rgt/";
    cmd += nbiot.IMEI;
    cmd += "/in,";
    cmd += String(contentLen);
    cmd += ",";
    cmd += content;
    publishMsg = cmd;
  }
};

extern DetectSystem detectSystem;

#endif
