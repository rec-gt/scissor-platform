
#ifndef WIFI_CONN_H
#define WIFI_CONN_H

#include <WiFi.h>
#include "Globals.h"


const char* ssid = "REC Guest - 16F";
const char* password = "guest@@2022";

IPAddress LocalIP(192, 168, 1, 184);
IPAddress Gateway(10, 236, 208, 1);
IPAddress SubNet(255, 255, 254, 0);

class WiFiConn {
  void init() {

    // if (!WiFi.config(LocalIP, Gateway, SubNet)) {
    //   Serial.println("STA Failed to configure");
    // }

    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED) {
      delay(1000);
      Serial.println("Connecting to WiFi...");
    }

    Serial.println("Connected to WiFi");
    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());
  }
};


extern WiFiConn wifiConn;

#endif