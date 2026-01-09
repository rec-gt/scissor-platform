#include "Lock.h"
#include "Counter.h"

#ifndef GLOBALS_H
#define GLOBALS_H

/*=== IoT ===*/
#define SerialIoT Serial1
#define IOT_MODULE_RESET_PIN 24

enum IOT_STATE {
  IOT_STATE_WAITING_INIT,

  IOT_STATE_WAITING_RESET,
  IOT_STATE_WAITING_RESET_HARDWARE,
  IOT_STATE_FINISH_RESET_HARDWARE,
  IOT_STATE_WAITING_RESET_SOFTWARE,
  IOT_STATE_FINISH_RESET_SOFTWARE,
  IOT_STATE_FINISH_RESET,

  IOT_STATE_WAITING_ASK_MODEL,
  IOT_STATE_FINISH_ASK_MODEL,

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
  IOT_STATE_WAITING_OPEN_MQTT,
  IOT_STATE_FINISH_OPEN_MQTT,
  IOT_STATE_WAITING_CONN_MQTT,
  IOT_STATE_FINISH_CONN_MQTT,
  IOT_STATE_WAITING_SUBS_MQTT_TOPIC,
  IOT_STATE_FINISH_SUBS_MQTT_TOPIC,

  IOT_STATE_FINISH_INIT,

  IOT_PIPELINE_INIT,
  IOT_PIPELINE_WAITING_PREPARE_PUBMSG,
  IOT_PIPELINE_FINISH_PREPARE_PUBMSG,
  IOT_PIPELINE_WAITING_PUBLISH,
  IOT_PIPELINE_FINISH_PUBLISH,
};

byte iotConnState = 0;
String iotSerialRecv = "";
String iotExtractedRecv = "";

String iotCmpStr = "";
int iotCmpStrIdx = 0;

String iotCSQ = "";
byte iotCSQTrial = 0;
String iotIMEI = "";
String iotCGATT = "";
String iotCEREG = "";
byte iotQueryCnt = 0;

bool iotDebugMode = false;

/*=== IoT MQTT ===*/
String mqttConnCmd = "";
String mqttSubsCmd = "";

Lock mqttPublishLock;
String mqttPublMsgPrepare = "";
String mqttPublMsgPayload = "";
String mqttSubsMsgContent = "";

bool forcePublishMode = false;

Counter iotCSQErrCnt;
Counter iotCGATTErrCnt;
Counter iotCEREGErrCnt;
Counter iotPublishErrCnt;

#endif