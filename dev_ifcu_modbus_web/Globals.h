
#ifndef GLOBALS_H
#define GLOBALS_H

/*=== For Main System ===*/
String serialRes = "";
String recvCmd = "";

/*=== For Modbus ===*/
RS485Class rs485(Serial2, 2, 3, 4);
ModbusRTUClientClass mbClient(rs485);

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
  IFCU_ACTION_INCREASE_TEMP,
  IFCU_ACTION_DECREASE_TEMP,
};

/*=== For Web Client ===*/
byte webClientSendBytes[7];  // 1 + 2 + 2 + 1 + 1
String webRecv = "";
byte webRecv[5] = {};

#endif