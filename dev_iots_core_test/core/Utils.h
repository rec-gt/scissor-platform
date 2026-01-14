#ifndef Utils_H
#define Utils_H

class Utils {
public:
  Utils(void){};

  bool isNumeric(const String& str) {
    for (size_t i = 0; i < str.length(); i++) {
      if (!isDigit(str.charAt(i))) {
        return false;
      }
    }
    return true;
  }

  byte hexCharToByte(char c) {
    if (c >= '0' && c <= '9') {
      return c - '0';
    } else if (c >= 'A' && c <= 'F') {
      return 10 + (c - 'A');
    } else if (c >= 'a' && c <= 'f') {
      return 10 + (c - 'a');
    } else {
      return 0;
    }
  }

  void serialInput() {
    while (Serial.available() > 0) {
      char c = Serial.read();
      if (c != '\r' && c != '\n') {
        serialInputCmd += c;
      }
      if (c == '\r') {
        if (serialInputCmd == F("RESET")) {
          iotModuleState = IOT_MODULE_WAITING_INIT;
        } else if (serialInputCmd == F("FORCE")) {
          iot.forcePublish();
        } else if (serialInputCmd == F("OPEN")) {
          Serial.println(F("Force Open:"));
          iot.printlnFlush((F("AT+QMTOPEN=0,iot.rec-gt.com,1880")));
        } else {
          iot.printlnFlush(serialInputCmd);
        }
        serialInputCmd = F("");
      }
    }
  }

  ~Utils(){};
};

extern Utils utils;

#endif
