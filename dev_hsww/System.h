#include "Enums.h"

#ifndef System_h
#define System_h

class System {
private:
  SystemStatus prevStatus = SYS_INIT;



public:
  System(){};
  ~System() {}
};

extern System system;

#endif