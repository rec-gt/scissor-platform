#ifndef KPS_H
#define KPS_H

#include "./SubGlobals.h"

class KPS {
private:

public:
  KPS(void) {}

  void loop() {
    analogOutputs[2].set(128);
  }

  ~KPS() {}
};

#endif