#define REQUEST_VALUES_LEN 4
#define RESPONSE_VALUES_LEN 7

#ifndef SlaveData_h
#define SlaveData_h

class SlaveData {
private:
  String name = "iFCU-16";

  uint16_t requestValues[REQUEST_VALUES_LEN] = {
    0,  // on/off
    0,  // set point temp
    0,  // mode
    0,  // speed
  };

  uint16_t responseValues[RESPONSE_VALUES_LEN] = {
    0,  // on/off
    0,  // mode
    0,  // speed
    0,  // room temp
    0,  // set point temp
    0,  // lower temp limit
    0,  // upper temp limit
  };

public:
  String dataStr = "";
  String responseStr = "";

  SlaveData() {}

  void getResponseStr() {
    this->responseStr = "";
    for (size_t i = 0; i < RESPONSE_VALUES_LEN; i++) {
      this->responseStr += responseValues[i];
      if (i < RESPONSE_VALUES_LEN - 1) {
        this->responseStr += ",";
      }
    }
  }

  ~SlaveData() {}
};

#endif