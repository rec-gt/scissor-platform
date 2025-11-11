#ifndef SUB_GLOBALS_H
#define SUB_GLOBALS_H

/*=== For Modbus ===*/
RS485Class rs485(Serial3, 0, 0, 22);
ModbusRTUClientClass mbClient(rs485);
// ModbusRTUServerClass mbServer(rs485);

#endif