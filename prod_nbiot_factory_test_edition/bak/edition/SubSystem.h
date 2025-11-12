#ifndef SubSystem_H
#define SubSystem_H

#include "Test.h"

Test test;

class SubSystem {
public:
  SubSystem(void) {}

  void init() {}

  void loop() {
    test.DO();
    test.AO();
  }

  ~SubSystem() {}
};

#endif