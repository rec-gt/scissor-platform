#ifndef GLOBALS_H
#define GLOBALS_H

/*=== Main System ===*/
uint16_t analogInputsMode;
String AIPayload = "";
String AOPayload = "";
String cmpStr = "";

/*=== NBIoT ===*/
String nbiotSerialRecv = "";
String nbIotConnCmd = "";
String nbiotSubsCmd = "";

String nbiotCSQ = "";
String nbiotIMEI = "";
String nbiotCGATT = "";
String nbiotCEREG = "";

String nbiotPubMsgPayload = "";
String nbiotPubMsgPrepare = "";
String nbiotPubMsgCommand = "";

String nbiotSubMsgContent = "";

String nbiotPubAck = "";
String nbiotSubAck = "";

/*=== Modbus485 ===*/
String rs485SerialRecv = "";

String rubbishStr = "";

#endif