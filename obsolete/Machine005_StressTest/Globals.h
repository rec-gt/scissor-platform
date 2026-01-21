#ifndef GLOBALS_H
#define GLOBALS_H

String serialRes = "";

String connStr = "";
String subStr = "AT+QMTSUB=0,1,rgt/861096060571706/out,0";
String subRecvContent = "";
String pubMsgContent = "{\"this\":\"is json\"}";
String pubMsgPrepare = "AT+QMTPUB=0,0,0,0,rgt/861096060571706/in,18";
String pubMsgForce = "";

#endif