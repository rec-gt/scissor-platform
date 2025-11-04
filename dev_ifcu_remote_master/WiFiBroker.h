#include <WiFi.h>

#ifndef WiFiBroker_H
#define WiFiBroker_H

// const char* apSSID = "ifcu-remote-16f-test";
// const char* apPassword = "ifcu-remote-16f-test";
const char* apSSID = "MIFI_1E40";
const char* apPassword = "1234567890";

IPAddress staticIP(192, 168, 1, 2);
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 255, 0);

class WiFiBroker {
private:
public:
  WiFiBroker(){};

  void init() {
    WiFi.mode(WIFI_AP);
    if (!WiFi.softAPConfig(staticIP, gateway, subnet)) {
      Serial.println("Failed to configure AP static IP");
    }

    WiFi.softAP(apSSID, apPassword);
    Serial.println("Access Point started");
    Serial.print("Master AP IP: ");
    Serial.println(WiFi.softAPIP());
  }

  void loop() {
  }

  ~WiFiBroker(){};
};

extern WiFiBroker wifiBroker;

#endif