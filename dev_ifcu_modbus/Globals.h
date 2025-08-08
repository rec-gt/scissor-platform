
#ifndef GLOBALS_H
#define GLOBALS_H

/*=== For Modbus ===*/
RS485Class rs485(Serial1, 2, 3, 4);
ModbusRTUClientClass mbClient(rs485);

/*=== For NBIoT ===*/
String serialRes = "";
String connStr = "";
String subStr = "AT+QMTSUB=0,1,rgt/861096060571706/out,0";
String subRecvContent = "";
String pubMsgContent = "{\"this\":\"is json\"}";
String pubMsgPrepare = "AT+QMTPUB=0,0,0,0,rgt/861096060571706/in,18";
String pubMsgForce = "";


#endif