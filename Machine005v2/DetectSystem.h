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
    publishMsgContent = "{\"csq\":";
    publishMsgContent += nbiot.CSQ;
    publishMsgContent += ",";
    publishMsgContent += "\"cgatt\":";
    publishMsgContent += nbiot.CGATT;
    publishMsgContent += ",";
    publishMsgContent += "\"cereg\":\"";
    publishMsgContent += nbiot.CEREG;
    publishMsgContent += "\"";
    publishMsgContent += ",";
    publishMsgContent += "\"din\":";
    publishMsgContent += String(sensorManager.sensorStatusX8);
    publishMsgContent += ",";
    publishMsgContent += "\"dout\":";
    publishMsgContent += String(sensorManager.sensorStatusX4);
    publishMsgContent += "}";

    int contentLen = publishMsgContent.length();

    publishMsg = "AT+QMTPUB=0,0,0,0,rgt/";
    publishMsg += nbiot.IMEI;
    publishMsg += "/in,";
    publishMsg += String(contentLen);
    publishMsg += ",";
    publishMsg += publishMsgContent;
  }
};

extern DetectSystem detectSystem;

#endif
