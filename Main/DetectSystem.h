#include "Arduino.h"
#include "SystemEnums.h"
#include "DisplayOLED.h"

class DetectSystem {
private:
  SystemStatus status = INIT;
  SystemStatus lastStatus = INIT;

public:
  SystemStatus getStatus() {
    return this->status;
  }

  void setStatus(SystemStatus status) {
    if (status == this->lastStatus) {
      return;
    }

    switch (status) {
      case RUNNING:
        displayOLED.print("", "系統運作中", "", 2);
        break;
      case STOPPED:
        displayOLED.print("偵測到障礙物", "系統暫停運作", "", 3);
        break;
    }

    this->lastStatus = status;
    this->status = status;

    Serial.print("System current status: ");
    Serial.println(STATUS_STR[status]);
  }
};