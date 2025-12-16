#ifndef WIFI_SERVICE_H
#define WIFI_SERVICE_H
#include <WiFi.h>
#include "Globals.h"

class WiFiService {
private:
  uint16_t prevMillis = millis();

  void connect() {
    /* ===== WiFi Connected ===== */
    if (WiFi.status() == WL_CONNECTED) {
      wifiConnStatus = WIFI_CONNECTED;
      Serial.println("WiFi connected..!");
      Serial.print("Got IP: ");
      Serial.println(WiFi.localIP());
    } else {
      /* ===== WiFi Disconnected ===== */
      if (wifiConnStatus == WIFI_DISCONNECTED) {
        Serial.println("Connecting to ");
        Serial.println(WIFI_SSID);

        WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
        wifiConnStatus = WIFI_INITIATED_CONN;
      } else if (wifiConnStatus == WIFI_INITIATED_CONN) {
        uint16_t currMillis = millis();
        if (currMillis - this->prevMillis >= 1000) {
          this->prevMillis = currMillis;
          Serial.print(".");
        }
      }
    }
  }

public:
  WiFiService() {}

  void loop() {
    this->connect();
  }

  bool isConnected() {
    return wifiConnStatus == WIFI_CONNECTED;
  }

  ~WiFiService() {}
};

extern WiFiService wifiService;

#endif