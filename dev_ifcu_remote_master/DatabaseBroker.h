#include "SlaveData.h"

#ifndef DatabaseBroker_h
#define DatabaseBroker_h


class DatabaseBroker {
private:
  SlaveData *slaveData;
  String responseJSONStr = "";

public:
  DatabaseBroker(SlaveData *slaveData)
    : slaveData(slaveData){};

  void getResponseJSONStr() {
    this->responseJSONStr = "";

    this->responseJSONStr += "[";
    for (size_t i = 0; i < sizeof(this->slaveData); i++) {
      this->slaveData[i].getResponseStr();
      this->responseJSONStr += "[";
      this->responseJSONStr += this->slaveData[i].responseStr;
      this->responseJSONStr += "],";
    }
    this->responseJSONStr += "]";
  }

  ~DatabaseBroker() {}
};

#endif