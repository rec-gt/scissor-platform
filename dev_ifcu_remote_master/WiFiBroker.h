#include <WiFi.h>
#include "Globals.h"
#include "AutoTimer.h"

#ifndef WiFiBroker_H
#define WiFiBroker_H

#define MAX_SLAVES 5
#define SERVER_PORT 80

const char* apSSID = "ifcu-remote-16f-test";
const char* apPassword = "ifcu-remote-16f-test";

IPAddress staticIP(192, 168, 1, 100);
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 255, 0);

class WiFiBroker {
private:
public:
  WiFiBroker(){};

  void init() {
  }

  void loop() {
  }

  ~WiFiBroker(){};
};

extern WiFiBroker wifiBroker;

#endif