#include "Arduino.h"
#include "SystemEnums.h"
#include "DisplayOLED.h"
#include "SensorManager.h"
#include "TrafficLight.h"
#include "NBIoT.h"

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
    String content = "{\"csq\":" + nbiot.CSQ + "," + "\"cgatt\":" + nbiot.CGATT + "," + "\"cereg\":\"" + nbiot.CEREG + "\"" + "," + "\"din\":" + String(sensorManager.sensorStatusX8) + "," + "\"dout\":"
                     + String(sensorManager.sensorStatusX4) + "}";
    int contentLen = content.length();
    String cmd = "AT+QMTPUB=0,0,0,0,rgt/" + String(nbiot.IMEI) + "/in," + String(contentLen) + "," + content;

    nbiot.setPublishMsg(cmd);
  }
};

extern DetectSystem detectSystem;

#endif
