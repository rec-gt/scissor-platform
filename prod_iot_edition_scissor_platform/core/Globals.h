#ifndef GLOBALS_H
#define GLOBALS_H

#include "./DigitalInput.h"
#include "./DigitalOutput.h"
#include "./AnalogInput.h"
#include "./AnalogOutput.h"
#include "./DryContact.h"
#include "./Toggle.h"
#include "./Timer.h"
#include "./Counter.h"
#include "./Watchdog.h"

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
String SWPayload = "";

/*=== IoT (4G & NB-IoT) ===*/
#define SerialIoT Serial1
#define IOT_MODULE_RESET_PIN 24
#define IOT_MODEL_EC800K "EC800K"
#define IOT_MODEL_BC260Y_CN "Quectel_BC260Y-CN"

enum IOT_MODULE_STATE {
  IOT_MODULE_WAITING_INIT,

  IOT_MODULE_WAITING_RESET,
  IOT_MODULE_WAITING_RESET_HARDWARE,
  IOT_MODULE_FINISH_RESET_HARDWARE,
  IOT_MODULE_WAITING_CONFIRM_BAUDRATE,
  IOT_MODULE_FINISH_CONFIRM_BAUDRATE,

  IOT_MODULE_FINISH_RESET,

  IOT_MODULE_WAITING_GET_INFO,
  IOT_MODULE_FINISH_GET_INFO,
  IOT_MODULE_WAITING_GET_MODEL,
  IOT_MODULE_FINISH_GET_MODEL,

  IOT_MODULE_FINISH_INIT,

  IOT_MODULE_END_OF_STATE,
};

enum IOT_CONN_STATE {
  IOT_CONN_WAITING_INIT,

  IOT_CONN_WAITING_ASSIGN_IP,
  IOT_CONN_FINISH_ASSIGN_IP,
  IOT_CONN_WAITING_CONFIG,
  IOT_CONN_FINISH_CONFIG,
  IOT_CONN_WAITING_CSQ,
  IOT_CONN_FINISH_CSQ,
  IOT_CONN_WAITING_CGATT,
  IOT_CONN_FINISH_CGATT,
  IOT_CONN_WAITING_CEREG,
  IOT_CONN_FINISH_CEREG,
  IOT_CONN_WAITING_OPEN_MQTT,
  IOT_CONN_FINISH_OPEN_MQTT,
  IOT_CONN_WAITING_CONN_MQTT,
  IOT_CONN_FINISH_CONN_MQTT,
  IOT_CONN_WAITING_SUBS_MQTT_TOPIC,
  IOT_CONN_FINISH_SUBS_MQTT_TOPIC,

  IOT_CONN_FINISH_INIT,

  IOT_CONN_END_OF_STATE,
};

enum IOT_MQTT_MSG_STATE {
  /* === MQTT Message State Management === */
  IOT_MQTT_MSG_LOOP_START,
  IOT_MQTT_MSG_WAITING_PUBLISH,
  IOT_MQTT_MSG_WAITING_PUBLISH_ACK,
  IOT_MQTT_MSG_FINISH_PUBLISH,
};

uint32_t iotSerialBaudRates[] = { 9600, 115200 };
size_t iotSerialBaudRateIdx = 0;

byte iotModuleState = IOT_MODULE_WAITING_INIT;
byte iotConnState = IOT_CONN_WAITING_INIT;
byte iotMqttMsgState = IOT_CONN_WAITING_INIT;

int iotParseIdx = 0;

String iotSerialRecv = "";

String iotModel = "";
String iotIMEI = "";
String iotCSQ = "";
String iotCGATT = "";
String iotCEREG = "";

/*=== IoT MQTT ===*/
String mqttConnCmd = "";
String mqttSubsCmd = "";

Toggle mqttPublishLock;
Toggle mqttForcePublMode;

String mqttPublMsgPrepare = "";
String mqttPublMsgPayload = "";
String mqttSubsMsgContent = "";

Counter iotResetHardwareCnt;

Timer iotModuleTimer;
Timer iotStateTimer;
Timer iotQueryTimer;

Watchdog iotSoftWatchdog(45000UL);

/*=== rs485 ===*/
String rs485SerialRecv = "";

/*=== Debug ===*/
String debugStr = "";
String serialInputCmd = "";

#endif