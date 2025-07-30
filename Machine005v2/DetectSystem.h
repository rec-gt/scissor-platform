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

    this->lastStatus = status;
    this->status = status;

    Serial.println(STATUS_STR[status]);
  }

  bool is(SystemStatus targetStatus) {
    return this->status == targetStatus;
  }

  void setPublishMsg() {
    publishMsgContent = "{\"csq\":";
    publishMsgContent.concat(nbiot.CSQ);
    publishMsgContent.concat(",");
    publishMsgContent.concat("\"cgatt\":");
    publishMsgContent.concat(nbiot.CGATT);
    publishMsgContent.concat(",");
    publishMsgContent.concat("\"cereg\":\"");
    publishMsgContent.concat(nbiot.CEREG);
    publishMsgContent.concat("\"");
    publishMsgContent.concat(",");
    publishMsgContent.concat("\"din\":");
    publishMsgContent.concat(String(sensorManager.sensorStatusX8));
    publishMsgContent.concat(",");
    publishMsgContent.concat("\"dout\":");
    publishMsgContent.concat(String(sensorManager.sensorStatusX4));
    publishMsgContent.concat(",");
    publishMsgContent.concat("\"sw\":");
    publishMsgContent.concat(String(this->status));
    publishMsgContent.concat("}");

    int contentLen = publishMsgContent.length();

    publishMsg = "AT+QMTPUB=0,0,0,0,rgt/";
    publishMsg.concat(nbiot.IMEI);
    publishMsg.concat("/in,");
    publishMsg.concat(String(contentLen));
    publishMsg.concat(",");
    publishMsg.concat(publishMsgContent);
  }
};

extern DetectSystem detectSystem;

#endif
