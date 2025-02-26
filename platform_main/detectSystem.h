#include "Arduino.h"
#include "SystemEnums.h"

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

    this->lastStatus = status;
    this->status = status;

    Serial.print("System current status: ");
    Serial.println(STATUS_STR[status]);
  }
};