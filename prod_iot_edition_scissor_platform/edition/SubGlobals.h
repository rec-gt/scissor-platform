#ifndef SUB_GLOBALS_H
#define SUB_GLOBALS_H

#include <ArduinoRS485.h>
#include <ArduinoModbus.h>
#include <EEPROM.h>

#define RS485Serial Serial3
#define RS485_RE_DE_PIN 22

/*=== Config AI Resolution ===*/
void configAnalogInputResolution(bool r = 0) {  // 0 = 1024, 1 = 4096
  for (size_t i = 0; i < AI_NUMS; i++) {
    analogInputs[i].setResolution(r);
  }
}

/*=== For Modbus ===*/
RS485Class recommendedStandard485(RS485Serial, 0, 0, RS485_RE_DE_PIN);
ModbusRTUClientClass mbRtuClient(recommendedStandard485);
// ModbusRTUServerClass mbServer(recommendedStandard485);

/*=== For EEPROM ===*/
enum EEPROM_ADDRESS {
  EEP_ADDR_THRESHOLD_DISTANCE,
  EEP_ADDR_ESCAPE_COUNT_DOWN,
};

/*=== For SubSystem ===*/
uint16_t sensorThresholdDistance = 1000;
uint16_t escapeCountDown = 5;

#endif