
#ifndef GLOBALS_H
#define GLOBALS_H
#include <ModbusMaster.h>
#include <WiFi.h>
#include <EEPROM.h>
#include <WebServer.h>
#include "Memory.h"

#define WIFI_SSID_ADDRESS 0
#define WIFI_SSID_LEN 32  // 0-31
#define WIFI_PASSWORD_ADDRESS 32
#define WIFI_PASSWORD_LEN 32  // 32-63

/* === WiFi Service === */
// const char* WIFI_SSID = "RGT-iFCU-Remote-Controller-16F";
// const char* WIFI_PASSWORD = "n5npy#6gfe";
// const char* WIFI_SSID = "REC Guest - 16F";
// const char* WIFI_PASSWORD = "guest@@2022";
// String WIFI_SSID = "RGT-iFCU-Remote-Controller-16F";
// String WIFI_PASSWORD = "n5npy#6gfe";
String WIFI_SSID = "";
String WIFI_PASSWORD = "";

enum WIFI_CONN_STATUS {
  WIFI_DISCONNECTED,
  WIFI_INITIATED_CONN,
  WIFI_CONNECTED
};

byte wifiConnStatus = WIFI_DISCONNECTED;

/* === Modbus Service === */
byte IFCU_SLAVE_ID = 1;
ModbusMaster mbNode;
uint8_t mbResult = mbNode.ku8MBSuccess;

/* === WebServer Service === */
bool restartServerFlag = true;
String jsonStr = "";

/* === iFCU Parameters === */
constexpr byte IR_SIZE = 11;
uint16_t READ_DATA[IR_SIZE] = {};

constexpr byte WRITE_DATA_SIZE = 7;
uint16_t WRITE_DATA[WRITE_DATA_SIZE] = {};
uint16_t WRITE_DATA_1[WRITE_DATA_SIZE] = {};
uint16_t WRITE_DATA_2[WRITE_DATA_SIZE] = {};

String QUEUE = "";
bool isSynced = false;  // remote ifcu status vs current ifcu status

#endif
