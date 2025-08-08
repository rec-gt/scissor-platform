
#ifndef GLOBALS_H
#define GLOBALS_H

RS485Class rs485(Serial1, 2, 3, 4);
ModbusRTUClientClass mbClient(rs485);

#endif