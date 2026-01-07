#ifndef GLOBALS_H
#define GLOBALS_H

/*=== IoT ===*/
#define SerialIoT Serial1

enum IOT_STATE {
  IOT_STATE_WAITING_INIT,

  IOT_STATE_WAITING_RESET,
  IOT_STATE_WAITING_RESET_HARDWARE,
  IOT_STATE_FINISH_RESET_HARDWARE,
  IOT_STATE_WAITING_RESET_SOFTWARE,
  IOT_STATE_FINISH_RESET_SOFTWARE,
  IOT_STATE_FINISH_RESET,

  IOT_STATE_WAITING_RDY,
  IOT_STATE_FINISH_RDY,
  IOT_STATE_WAITING_IP,
  IOT_STATE_FINISH_IP,
  IOT_STATE_WAITING_CONFIG,
  IOT_STATE_FINISH_CONFIG,
  IOT_STATE_WAITING_CSQ,
  IOT_STATE_FINISH_CSQ,
  IOT_STATE_WAITING_CGATT,
  IOT_STATE_FINISH_CGATT,
  IOT_STATE_WAITING_CEREG,
  IOT_STATE_FINISH_CEREG,
  IOT_STATE_WAITING_OPEN,
  IOT_STATE_FINISH_OPEN,
  IOT_STATE_WAITING_CONN,
  IOT_STATE_FINISH_CONN,
  IOT_STATE_WAITING_SUB,
  IOT_STATE_FINISH_SUB,

  IOT_STATE_FINISH_INIT,
};

byte iotConnState = 0;
byte iotResetState = 0;
String iotSerialRecv = "";
String iotExtractedRecv = "";

String iotCmpStr = "";
int iotCmpStrIdx = 0;

String iotCSQ = "";
byte iotCSQTrial = 0;
String iotIMEI = "";
String iotCGATT = "";
String iotCEREG = "";

/*=== IoT MQTT ===*/
String mqttConnCmd = "";
String mqttSubsCmd = "";

bool mqttPublMsgPayloadLock = false;
String mqttPublMsgPrepare = "";
String mqttPublMsgPayload = "";
String mqttPublMsgCommand = "";
String mqttSubsMsgContent = "";

String mqttPubAck = "";
String mqttSubAck = "";

bool iotSoftReset = false;
bool iotDebugMode = false;

#endif