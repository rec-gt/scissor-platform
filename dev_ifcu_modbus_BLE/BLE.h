#include "AsyncTimer.h"
#include "Utils.h"
#include "Watchdog.h"
#include "Globals.h"

#ifndef BLE_h
#define BLE_h

#define BLESerial Serial3

class BLE {
private:
  bool finishInit = false;

  bool debugMode = false;

  void clearSerialBuffer() {
    while (BLESerial.read() > 0) { delay(1); };
  }

  void clearResBuffer() {
    serialRes = "";
  }

  void printlnFlush(String cmd, unsigned int delayTime = 2) {
    BLESerial.println(cmd);
    BLESerial.flush();
    delay(delayTime);
  }

public:
  BLE() {
    BLESerial.begin(4800);
  }

  void listen() {
    if (BLESerial.available() > 0) {
      while (BLESerial.available() > 0) {
        char c = BLESerial.read();

        Serial.print(c);

        if (c != '\r' && c != '\n') {
          serialRes += c;
        }

        if (c == '\r') {
          this->clearResBuffer();
        }
      }
    }
  }

  ~BLE() {}
};

extern BLE ble;

#endif