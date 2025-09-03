#include <WiFi.h>

#ifndef WiFiBroker_H
#define WiFiBroker_H

const char* apSSID = "ifcu-remote-16f-test";
const char* apPassword = "ifcu-remote-16f-test";

IPAddress staticIP(192, 168, 1, 100);
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 255, 0);

WiFiServer server(80);

class WiFiBroker {
private:
public:
  WiFiBroker(){};

  void init() {
   
  }

  void loop() {
    this->handleNewClientJoin();
  }

  void handleNewClientJoin() {
   
  }

  ~WiFiBroker(){};
};

extern WiFiBroker wifiBroker;

#endif