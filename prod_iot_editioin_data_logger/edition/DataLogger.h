#ifndef DataLogger_H
#define DataLogger_H

#include "./SubRS485.h"

class DataLogger {
private:

public:
  DataLogger(void) {}

  void log(String& data) {
    subRS485.write(data);
  }

  ~DataLogger() {}
};

extern DataLogger dataLogger;

#endif