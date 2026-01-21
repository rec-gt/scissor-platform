#include "Arduino.h"
#include "SystemEnums.h"
// #include "DisplayOLED.h"
#include "SensorManager.h"
#include "TrafficLight.h"
#include "NBIoT.h"
#include "Globals.h"
#include "DisplayClient.h"

#ifndef detectSystem_h
#define detectSystem_h

DisplayClient displayClient;

class DetectSystem {
private:
  SystemStatus status = SYS_INIT;
  SystemStatus lastStatus = SYS_INIT;
  unsigned long prevMillis = 0;
  unsigned long prevMillisDisplay = 0;

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

    nbiotPubMsgPayload = "{\"csq\":";
    nbiotPubMsgPayload.concat(nbiotCSQ);
    nbiotPubMsgPayload.concat(",");
    nbiotPubMsgPayload.concat("\"cgatt\":");
    nbiotPubMsgPayload.concat(nbiotCGATT);
    nbiotPubMsgPayload.concat(",");
    nbiotPubMsgPayload.concat("\"cereg\":\"");
    nbiotPubMsgPayload.concat(nbiotCEREG);
    nbiotPubMsgPayload.concat("\"");
    nbiotPubMsgPayload.concat(",");
    nbiotPubMsgPayload.concat("\"din\":");
    nbiotPubMsgPayload.concat(String(sensorManager.sensorStatusX8));
    nbiotPubMsgPayload.concat(",");
    nbiotPubMsgPayload.concat("\"dout\":");
    nbiotPubMsgPayload.concat(String(sensorManager.sensorStatusX4));
    nbiotPubMsgPayload.concat("}");

    int contentLen = nbiotPubMsgPayload.length();

    nbiotPubMsgPrepare = "AT+QMTPUB=0,0,0,0,rgt/";
    nbiotPubMsgPrepare.concat(nbiotIMEI);
    nbiotPubMsgPrepare.concat("/in,");
    nbiotPubMsgPrepare.concat(String(contentLen));

    nbiotPubMsgCommand = nbiotPubMsgPrepare;
    nbiotPubMsgCommand.concat(",");
    nbiotPubMsgCommand.concat(nbiotPubMsgPayload);
  }

  void handleDisplayContent() {
    if (millis() - this->prevMillisDisplay > 2000) {
      {
        int csq = nbiotCSQ.toInt();
        displayClient.prepareBuffer(nbiot.connState, csq, 0, 0, sensorManager.laserSensors, sensorManager.laserSensors, 65535);
        displayClient.sendBuffer();
      }
      this->prevMillisDisplay = millis();
    }
  }
};

extern DetectSystem detectSystem;

#endif
