#include "SlaveData.h"

#ifndef DatabaseBroker_h
#define DatabaseBroker_h

SlaveData *slaveData;

class DatabaseBroker {
private:
  String responseJSONStr = "";

public:
  DatabaseBroker(SlaveData *slaveData)
    : slaveData(slaveData){};

  void getResponseJSONStr() {
    this->responseJSONStr = "";

    this->responseJSONStr += "[";
    for (size_t i = 0; i < sizeof(this->slaveData); i++) {
      this->slaveData.getResponseStr();
      this->responseJSONStr += "[";
      this->responseJSONStr += this->slaveData.responseStr;
      this->responseJSONStr += "],";
    }
    this->responseJSONStr += "]";
  }

  ~DatabaseBroker() {}
};

#endif