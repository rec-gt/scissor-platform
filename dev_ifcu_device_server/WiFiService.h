#ifndef WIFI_SERVICE_H
#define WIFI_SERVICE_H

#include "./Globals.h"
#include "./Timer.h"

Timer wifiTimer;

class WiFiService {
private:
  uint32_t prevMillis = millis();

  void connect() {
    /* ===== WiFi Connected ===== */
    if (WiFi.status() == WL_CONNECTED) {
      wifiConnStatus = WIFI_CONNECTED;
    } else {
      /* ===== WiFi Disconnected ===== */
      if (wifiConnStatus == WIFI_DISCONNECTED) {
        WiFi.disconnect(true);
        memory.readStr(WIFI_SSID_ADDRESS, WIFI_SSID_LEN, WIFI_SSID);
        memory.readStr(WIFI_PASSWORD_ADDRESS, WIFI_PASSWORD_LEN, WIFI_PASSWORD);
        Serial.println("Connecting to ");
        Serial.println(WIFI_SSID);
        WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
        wifiConnStatus = WIFI_INITIATED_CONN;
      } else if (wifiConnStatus == WIFI_INITIATED_CONN) {
        uint32_t currMillis = millis();
        if (currMillis - this->prevMillis >= 1000) {
          this->prevMillis = currMillis;
          Serial.print(".");
        }

        if (wifiTimer.autoTimeout(10000)) {
          wifiConnStatus = WIFI_DISCONNECTED;
        }
      } else {
        wifiConnStatus = WIFI_DISCONNECTED;
      }

      restartServerFlag = true;
    }
  }

public:
  WiFiService() {}

  void init() {
    WiFi.mode(WIFI_AP_STA);
    WiFi.softAPConfig(IPAddress(192, 168, 1, 1), IPAddress(192, 168, 1, 1), IPAddress(255, 255, 255, 0));
    WiFi.softAP("RGT-IFCU-DEVICE-001", "RECrec26198888");
  }

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