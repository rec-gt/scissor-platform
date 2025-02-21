#include "Arduino.h"

enum SystemStatus {
  STOPPED,
  RUNNING,
  ALLOW_10S,
  FAILURE
};

const char* STATUS_STR[] = { "STOPPED", "RUNNING", "ALLOW_10S", "FAILURE" };


class DetectSystem {
private:
  SystemStatus status = RUNNING;

public:
  SystemStatus getStatus() {
    return this->status;
  }

  void setStatus(SystemStatus status) {
    this->status = status;
    Serial.print("System current status: ");
    Serial.println(STATUS_STR[status]);
  }
};