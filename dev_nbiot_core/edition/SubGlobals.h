#ifndef SUB_GLOBALS_H
#define SUB_GLOBALS_H

#include <ArduinoRS485.h>
#include <ArduinoModbus.h>

#define RS485Serial Serial3
#define RS485_RE_DE_PIN 22

/*=== Config AI Resolution ===*/
void configAnalogInputResolution(bool r = 0) {  // 0 = 1024, 1 = 4096
  for (size_t i = 0; i < AI_NUMS; i++) {
    analogInputs[i].setResolution(0);
  }
}

/*=== For Modbus ===*/
RS485Class rs485(RS485Serial, 0, 0, RS485_RE_DE_PIN);
ModbusRTUClientClass mbRtuClient(rs485);
// ModbusRTUServerClass mbServer(rs485);

#endif