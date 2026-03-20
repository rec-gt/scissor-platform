
#ifndef GLOBALS_H
#define GLOBALS_H
#include <ModbusMaster.h>
#include <HTTPClient.h>
#include <WiFi.h>

/* === System Config === */
String DEVICE_NAME = "ifcu-001";

/* === WiFi Service === */
const char* WIFI_SSID = "REC Guest - 16F";  // Enter SSID here
const char* WIFI_PASSWORD = "guest@@2022";  // Enter Password here

enum WIFI_CONN_STATUS {
  WIFI_DISCONNECTED,
  WIFI_INITIATED_CONN,
  WIFI_CONNECTED
};

byte wifiConnStatus = WIFI_DISCONNECTED;

/* === HTTP Service === */
String serverName = "http://10.236.209.200/device/get";
String serverPath = "";

/* === Modbus Service === */
byte IFCU_SLAVE_ID = 1;
ModbusMaster mbNode;
uint8_t mbResult = mbNode.ku8MBSuccess;

/* === iFCU Parameters === */
constexpr byte IR_SIZE = 10;
uint16_t READ_DATA[IR_SIZE] = {};
uint16_t WRITE_DATA[4] = {};
uint16_t WRITE_DATA_BAK[4] = {};
String QUEUE = "";
bool isSynced = false;  // remote ifcu status vs current ifcu status

#endif
