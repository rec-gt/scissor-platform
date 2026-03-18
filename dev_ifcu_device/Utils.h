#ifndef Utils_H
#define Utils_H

class Utils {

public:
  void listenSerial() {
    while (Serial.available()) {
      char c = Serial.read();
      QUEUE += c;
    }
  }
};

extern Utils utils;

#endif