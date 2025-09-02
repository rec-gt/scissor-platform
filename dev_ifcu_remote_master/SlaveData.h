#define REQUEST_VALUES_LEN 4
#define RESPONSE_VALUES_LEN 7

#ifndef SlaveData_h
#define SlaveData_h

class SlaveData {
private:

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
  String name = "iFCU-16";

  String responseStr = "";

  SlaveData() {}

  ~SlaveData() {}
};

#endif