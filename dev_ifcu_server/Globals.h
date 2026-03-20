#ifndef GLOBALS_H
#define GLOBALS_H
#include <WiFi.h>
#include <WebServer.h>

/* === WiFi Service === */
const char* WIFI_SSID = "ASUS_A6";
const char* WIFI_PASSWORD = "n5npy#6gfe";

// const char* WIFI_SSID = "REC Guest - 16F";  // Enter SSID here
// const char* WIFI_PASSWORD = "guest@@2022";  // Enter Password here

enum WIFI_CONN_STATUS {
  WIFI_DISCONNECTED,
  WIFI_INITIATED_CONN,
  WIFI_CONNECTED
};

byte wifiConnStatus = WIFI_DISCONNECTED;


#endif
