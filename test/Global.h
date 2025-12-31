#ifndef GLOBALS_H
#define GLOBALS_H

/*=== IoT ===*/
byte iotConnState = 0;
String iotSerialRecv = "";

String iotCSQ = "";
String iotIMEI = "";
String iotCGATT = "";
String iotCEREG = "";

/*=== IoT MQTT ===*/
String mqttConnCmd = "";
String mqttSubsCmd = "";

bool mqttPublMsgLock = false;
String mqttPublMsgPrepare = "";
String mqttPublMsgPayload = "";
String mqttPublMsgCommand = "";
String mqttSubsMsgContent = "";

String mqttPubAck = "";
String mqttSubAck = "";

#endif