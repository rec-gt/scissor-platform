
#ifndef GLOBALS_H
#define GLOBALS_H
#include <ModbusMaster.h>
#include <WiFi.h>

/* === WiFi Service === */
// const char* WIFI_SSID = "ASUS_A6";
// const char* WIFI_PASSWORD = "n5npy#6gfe";
const char* WIFI_SSID = "REC Guest - 16F";
const char* WIFI_PASSWORD = "guest@@2022";

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

constexpr byte WRITE_DATA_SIZE = 6;
uint16_t WRITE_DATA[WRITE_DATA_SIZE] = {};
uint16_t WRITE_DATA_1[WRITE_DATA_SIZE] = {};
uint16_t WRITE_DATA_2[WRITE_DATA_SIZE] = {};

String QUEUE = "";
bool isSynced = false;  // remote ifcu status vs current ifcu status
uint32_t lastCommAt = millis();

#endif
