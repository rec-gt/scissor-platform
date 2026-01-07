#ifndef GLOBALS_H
#define GLOBALS_H

/*=== IoT ===*/
#define SerialIoT Serial1

byte iotConnState = 0;
String iotSerialRecv = "";

String iotCmpStr = "";
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