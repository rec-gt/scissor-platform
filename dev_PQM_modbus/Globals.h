
#ifndef GLOBALS_H
#define GLOBALS_H

/*=== For Modbus ===*/
RS485Class rs485(Serial3, 2, 3, 4);
ModbusRTUClientClass mbClient(rs485);


/*=== For NBIoT ===*/
String serialRes = "";
String connStr = "";
String subStr = "";
String subRecvContent = "";
String pubMsgContent = "";
String pubMsgPrepare = "";
String pubMsgForce = "";


#endif