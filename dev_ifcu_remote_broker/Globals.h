
#ifndef Globals_H
#define Globals_H

#define REQUEST_VALUES_LEN 4
#define RESPONSE_VALUES_LEN 7

int requestValues[REQUEST_VALUES_LEN] = {
  0,  // on/off
  0,  // set point temp
  0,  // mode
  0,  // speed
};

int responseValues[RESPONSE_VALUES_LEN] = {
  0,  // on/off
  0,  // room temp
  0,  // set point temp
  0,  // mode
  0,  // speed
  0,  // lower temp limit
  0,  // upper temp limit
};

#endif
