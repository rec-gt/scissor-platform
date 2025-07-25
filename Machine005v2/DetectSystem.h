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
      displayOLED.print("", "系統運作中", "", DISPLAY_SYS_RUNNING);
    }

    this->lastStatus = status;
    this->status = status;

    Serial.print("System current status: ");
    Serial.println(STATUS_STR[status]);
  }

  bool is(SystemStatus targetStatus) {
    return this->status == targetStatus;
  }

  void publishStatus(byte reason = 0) {
    // reason: 0 - send every 30s
    //         1 - force stop (aka obstacle enters 500mm range)
    //         2 - allow 10s control
    //         3 - recover back to running
    //         4 - obstacle enters 800mm range
    sensorManager.getSensorsStatus();

    byte trafficValue = 30;
    if (trafficLight.getStatus() == TRAFFIC_YELLOW) {
      trafficValue = 60;
    } else if (trafficLight.getStatus() == TRAFFIC_RED) {
      trafficValue = 90;
    }

    // String cmd = "AT+MQTTPUB=\"rgt/"
    //              + String(nbiot.IMEI) + "/in\",1,0,0,0,\"{\"seq\":1,\"csq\":"
    //              + String(nbiot.CSQ) + ",\"sw\":0,\"din\":"
    //              + String(sensorManager.sensorStatusX8) + ",\"dout\":"
    //              + String(sensorManager.sensorStatusX4) + ",\"ain\":["
    //              + String(this->getStatus()) + ","
    //              + trafficValue + ",0,0],\"aout\":[0,0,0,0]}\"";

    String cmd = "";
    
    if (reason == 0) {
      if (millis() - this->prevMillis > 30 * 1000) {  // send every 30s
        this->prevMillis = millis();
        // nbiot.sendCMDFast(cmd);
      }
    } else {
      // nbiot.sendCMDOnce(reason, cmd);
    }
  }
};

extern DetectSystem detectSystem;

#endif
