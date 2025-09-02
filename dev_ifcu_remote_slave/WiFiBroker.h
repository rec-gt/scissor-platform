#include <WiFi.h>
#include "Globals.h"
#include "AutoTimer.h"

#ifndef WiFiBroker_H
#define WiFiBroker_H

const char* ssid = "iFCU-16f-test-ckyt";
const char* password = "iFCU-16f-test-ckyt";

WiFiServer server(80);

AutoTimer timer;

class WiFiBroker {
private:
public:
  WiFiBroker(){};

  void init() {
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED) {
      delay(1000);
      Serial.println("Connecting to WiFi...");
    }

    Serial.println(WiFi.localIP());
  }

  ~WiFiBroker(){};
};

extern WiFiBroker wifiBroker;

#endif