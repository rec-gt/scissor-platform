
#ifndef GLOBALS_H
#define GLOBALS_H

/*=== For Modbus ===*/
RS485Class rs485(Serial2, 2, 3, 4);
ModbusRTUClientClass mbClient(rs485);

/*=== For NBIoT ===*/
String serialRes = "";
String connStr = "";
String subStr = "AT+QMTSUB=0,1,rgt/861096060571706/out,0";
String subRecvContent = "";
String pubMsgContent = "{\"this\":\"is json\"}";
String pubMsgPrepare = "AT+QMTPUB=0,0,0,0,rgt/861096060571706/in,18";
String pubMsgForce = "";


/*=== For IFCU ===*/
enum IFCU_ENUMS {
  IFCU_ON,
  IFCU_OFF,
  IFCU_MODE_AUTO_COOL,
  IFCU_MODE_MANUAL_COOL,
  IFCU_MODE_FAN_ONLY,
  IFCU_MODE_AUTO_HEAT,
  IFCU_MODE_MANUAL_HEAT,
  IFCU_FAN_SPEED_LOW,
  IFCU_FAN_SPEED_MEDIUM,
  IFCU_FAN_SPEED_HIGH,
  IFCU_FAN_SPEED_TEN_SPEED,
};

#endif