#include "Arduino.h"
#include "SystemEnums.h"
#include "DisplayOLED.h"

class DetectSystem {
private:
  SystemStatus status = SYS_INIT;
  SystemStatus lastStatus = SYS_INIT;

public:
  SystemStatus getStatus() {
    return this->status;
  }

  void set(SystemStatus status) {
    if (status == this->lastStatus) {
      return;
    }

    switch (status) {
      case SYS_RUNNING:
        displayOLED.print("", "系統運作中", "", 200);
        break;
      case SYS_STOPPED:
        // displayOLED.print("偵測到障礙物", "系統暫停運作", "", 100);
        break;
    }

    this->lastStatus = status;
    this->status = status;

    Serial.print("System current status: ");
    Serial.println(STATUS_STR[status]);
  }

  bool is(SystemStatus targetStatus) {
    return this->status == targetStatus;
  }
};