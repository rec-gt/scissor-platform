#include <ArduinoRS485.h>
#include <ArduinoModbus.h>
#include <HardwareSerial.h>

#ifndef GLOBALS_H
#define GLOBALS_H

/*=== Config ===*/
byte IFCU_ID = 1;

/*=== For Modbus ===*/
RS485Class rs485(Serial1, 2, 3, 4);
ModbusRTUClientClass mbClient(rs485);

#endif