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
    if (this->status == SYS_RUNNING) {
      sensorManager.sensorStatusX4 |= ~(1 << 3);  // status = true, running
    } else {
      sensorManager.sensorStatusX4 &= ~(1 << 3);  // status = false, stopped
    }

    pubMsgContent = "{\"csq\":";
    pubMsgContent.concat(nbiot.CSQ);
    pubMsgContent.concat(",");
    pubMsgContent.concat("\"cgatt\":");
    pubMsgContent.concat(nbiot.CGATT);
    pubMsgContent.concat(",");
    pubMsgContent.concat("\"cereg\":\"");
    pubMsgContent.concat(nbiot.CEREG);
    pubMsgContent.concat("\"");
    pubMsgContent.concat(",");
    pubMsgContent.concat("\"din\":");
    pubMsgContent.concat(String(sensorManager.sensorStatusX8));
    pubMsgContent.concat(",");
    pubMsgContent.concat("\"dout\":");
    pubMsgContent.concat(String(sensorManager.sensorStatusX4));
    pubMsgContent.concat("}");

    int contentLen = pubMsgContent.length();

    pubMsgPrepare = "AT+QMTPUB=0,0,0,0,rgt/";
    pubMsgPrepare.concat(nbiot.IMEI);
    pubMsgPrepare.concat("/in,");
    pubMsgPrepare.concat(String(contentLen));

    pubMsgForce = pubMsgPrepare;
    pubMsgForce.concat(",");
    pubMsgForce.concat(pubMsgContent);
  }
};

extern DetectSystem detectSystem;

#endif
