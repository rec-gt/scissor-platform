#ifndef SUB_GLOBALS_H
#define SUB_GLOBALS_H

#include <ArduinoRS485.h>
#include <ArduinoModbus.h>

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

/*=== For System ===*/
enum HR_ENUMS {
  HR_01,
  HR_02,
  HR_03,
  HR_04,
  HR_05,
  HR_06,
  HR_07,
  HR_08,
  HR_09,
  HR_10,
};
static constexpr uint16_t PARAMETERS_SIZE = 10;
float holdingRegisterValues[PARAMETERS_SIZE] = {};
float prevHoldingRegisterValues[PARAMETERS_SIZE] = {};
bool eventTriggerFlag = false;

Timer deviceTimer(10000UL);

#endif