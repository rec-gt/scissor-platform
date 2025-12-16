
#ifndef GLOBALS_H
#define GLOBALS_H

/* === WiFi Service === */
const char* WIFI_SSID = "REC Guest";        // Enter SSID here
const char* WIFI_PASSWORD = "guest@@2022";  // Enter Password here

enum WIFI_CONN_STATUS {
  WIFI_DISCONNECTED,
  WIFI_INITIATED_CONN,
  WIFI_CONNECTED
};

byte wifiConnStatus = WIFI_DISCONNECTED;

/* === mDNS Service === */
const char* mDNSHostname = "ifcu-mdns-1";

enum MDNS_CONN_STATUS {
  MDNS_EMPTY,
  MDNS_STARTED,
  MDNS_PROBE,
  MDNS_PROBE_FINISH,
};

byte mDNSStatus = MDNS_EMPTY;

const char* gatewayHostname = "ifcuweb";

String gatewayIPAddress = "";

/* === Modbus Service === */
constexpr byte IR_SIZE = 17;
constexpr byte HR_SIZE = 17;
uint16_t IR_DATABASE[IR_SIZE] = {};
uint16_t HR_DATABASE[HR_SIZE] = {};

#endif
