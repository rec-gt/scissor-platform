#include "SlaveData.h"

#ifndef DatabaseBroker_h
#define DatabaseBroker_h


class DatabaseBroker {
private:
  SlaveData *slaveData;

public:
  String responseJSONStr = "";

  DatabaseBroker(SlaveData *slaveData)
    : slaveData(slaveData){};

  void getResponseJSONStr() {
    this->responseJSONStr = "";

    this->responseJSONStr += "[";
    for (size_t i = 0; i < sizeof(this->slaveData); i++) {
      this->responseJSONStr += "[";
      this->responseJSONStr += this->slaveData[i].responseStr;
      this->responseJSONStr += "],";
    }
    this->responseJSONStr += "]";
  }

  void setSlaveResponse(int i, String data) {
    this->slaveData[i].responseStr = "";
    this->slaveData[i].responseStr += this->slaveData[i].name;
    this->slaveData[i].responseStr += ",";
    this->slaveData[i].responseStr += data;
  }

  ~DatabaseBroker() {}
};

#endif