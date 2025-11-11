#ifndef SUB_GLOBALS_H
#define SUB_GLOBALS_H

#include <ArduinoRS485.h>
#include <ArduinoModbus.h>

#define RS485Serial Serial3;
#define RS485_RE_DE_PIN 22;

/*=== For Modbus ===*/
RS485Class rs485(RS485Serial, 0, 0, RS485_RE_DE_PIN);
ModbusRTUClientClass mbRtuClient(rs485);
// ModbusRTUServerClass mbServer(rs485);

#endif