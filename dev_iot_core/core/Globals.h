#ifndef GLOBALS_H
#define GLOBALS_H

#include "./DigitalInput.h"
#include "./DigitalOutput.h"
#include "./AnalogInput.h"
#include "./AnalogOutput.h"
#include "./DryContact.h"
#include "./Lock.h"
#include "./Counter.h"

/*=== Main System ===*/
#define DI_PIN_1 32
#define DI_PIN_2 33
#define DI_PIN_3 34
#define DI_PIN_4 35
#define DI_PIN_5 36
#define DI_PIN_6 37
#define DI_PIN_7 38
#define DI_PIN_8 39

#define DO_PIN_1 40
#define DO_PIN_2 41
#define DO_PIN_3 42
#define DO_PIN_4 43
#define DO_PIN_5 44
#define DO_PIN_6 45
#define DO_PIN_7 46
#define DO_PIN_8 47

#define AI_PIN_1 A0
#define AI_PIN_2 A1
#define AI_PIN_3 A2
#define AI_PIN_4 A3
#define AI_PIN_5 A4
#define AI_PIN_6 A5
#define AI_PIN_7 A6
#define AI_PIN_8 A7
#define AI_PIN_9 A8
#define AI_PIN_10 A9
#define AI_PIN_11 A10
#define AI_PIN_12 A11

#define AO_PIN_1 4
#define AO_PIN_2 5
#define AO_PIN_3 6
#define AO_PIN_4 7

#define DRY_CONTACT_PIN_1 27
#define DRY_CONTACT_PIN_2 28
#define DRY_CONTACT_PIN_3 29
#define DRY_CONTACT_PIN_4 30
#define DRY_CONTACT_PIN_5 31

#define DI_NUMS 8
#define DO_NUMS 8
#define AI_NUMS 12
#define AO_NUMS 4
#define DRY_CONTACT_NUMS 5

// Higher level IO reference
#define DI_1 0
#define DI_2 1
#define DI_3 2
#define DI_4 3
#define DI_5 4
#define DI_6 5
#define DI_7 6
#define DI_8 7

#define DO_1 0
#define DO_2 1
#define DO_3 2
#define DO_4 3
#define DO_5 4
#define DO_6 5
#define DO_7 6
#define DO_8 7

#define AI_1 0
#define AI_2 1
#define AI_3 2
#define AI_4 3
#define AI_5 4
#define AI_6 5
#define AI_7 6
#define AI_8 7
#define AI_9 8
#define AI_10 9
#define AI_11 10
#define AI_12 11

#define AO_1 0
#define AO_2 1
#define AO_3 2
#define AO_4 3

DigitalInput digitalInputs[DI_NUMS] = {
  DigitalInput(DI_PIN_1),
  DigitalInput(DI_PIN_2),
  DigitalInput(DI_PIN_3),
  DigitalInput(DI_PIN_4),
  DigitalInput(DI_PIN_5),
  DigitalInput(DI_PIN_6),
  DigitalInput(DI_PIN_7),
  DigitalInput(DI_PIN_8),
};

DigitalOutput digitalOutputs[DO_NUMS]{
  DigitalOutput(DO_PIN_1),
  DigitalOutput(DO_PIN_2),
  DigitalOutput(DO_PIN_3),
  DigitalOutput(DO_PIN_4),
  DigitalOutput(DO_PIN_5),
  DigitalOutput(DO_PIN_6),
  DigitalOutput(DO_PIN_7),
  DigitalOutput(DO_PIN_8),
};

AnalogOutput analogOutputs[AO_NUMS]{
  AnalogOutput(AO_PIN_1),
  AnalogOutput(AO_PIN_2),
  AnalogOutput(AO_PIN_3),
  AnalogOutput(AO_PIN_4),
};

AnalogInput analogInputs[AI_NUMS] = {
  AnalogInput(AI_PIN_1, 1),
  AnalogInput(AI_PIN_2, 1),
  AnalogInput(AI_PIN_3, 1),
  AnalogInput(AI_PIN_4, 1),
  AnalogInput(AI_PIN_5, 1),
  AnalogInput(AI_PIN_6, 1),
  AnalogInput(AI_PIN_7, 1),
  AnalogInput(AI_PIN_8, 1),
  AnalogInput(AI_PIN_9, 1),
  AnalogInput(AI_PIN_10, 1),
  AnalogInput(AI_PIN_11, 1),
  AnalogInput(AI_PIN_12, 1)
};

DryContact dryContacts[DRY_CONTACT_NUMS] = {
  DryContact(DRY_CONTACT_PIN_1),
  DryContact(DRY_CONTACT_PIN_2),
  DryContact(DRY_CONTACT_PIN_3),
  DryContact(DRY_CONTACT_PIN_4),
  DryContact(DRY_CONTACT_PIN_5),
};

byte DIPayload = 0;
byte DOPayload = 0;
String AIPayload = "";
String AOPayload = "";

/*=== IoT (4G & NB-IoT) ===*/
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

int iotCmpStrIdx = 0;

String iotCSQ = "";
byte iotCSQTrial = 0;
String iotIMEI = "";
String iotCGATT = "";
String iotCEREG = "";
byte iotQueryCnt = 0;

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
Counter mqttPublishErrCnt;

/*=== rs485 ===*/
String rs485SerialRecv = "";

/*=== Debug ===*/
String debugStr = "";

#endif