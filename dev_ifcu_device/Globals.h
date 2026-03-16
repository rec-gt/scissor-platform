
#ifndef GLOBALS_H
#define GLOBALS_H
#include <ModbusMaster.h>
#include <HTTPClient.h>

/* === System Config === */
byte DEVICE_ID = 0;


/* === WiFi Service === */
const char* WIFI_SSID = "REC Guest - 16F";  // Enter SSID here
const char* WIFI_PASSWORD = "guest@@2022";  // Enter Password here

enum WIFI_CONN_STATUS {
  WIFI_DISCONNECTED,
  WIFI_INITIATED_CONN,
  WIFI_CONNECTED
};

byte wifiConnStatus = WIFI_DISCONNECTED;

/* === mDNS Service === */
const char* mDNSHostname = "esp32";

enum MDNS_CONN_STATUS {
  MDNS_EMPTY,
  MDNS_STARTED,
  MDNS_PROBE,
  MDNS_PROBE_FINISH,
};

byte mDNSStatus = MDNS_EMPTY;

const char* gatewayHostname = "ifcuweb";

String gatewayIPAddress = "";

/* === HTTP Service === */
HTTPClient http;

String httpReqGetHR = "";
String httpReqSetHR = "";

/* === Modbus Service === */
byte IFCU_SLAVE_ID = 1;
ModbusMaster mbNode;

constexpr byte IR_SIZE = 17;
constexpr byte HR_SIZE = 17;
uint16_t IR_DATABASE[IR_SIZE] = {};
uint16_t HR_DATABASE[HR_SIZE] = {};

String QUEUE = "";
uint16_t READ_DATA[HR_SIZE] = {};
uint16_t TMP_DATA[HR_SIZE] = {};
uint16_t WRITE_DATA[HR_SIZE] = {};

#endif
