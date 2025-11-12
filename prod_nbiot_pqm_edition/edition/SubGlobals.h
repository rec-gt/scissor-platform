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

enum HR_ENUMS {
  HR_UAN,
  HR_UBN,
  HR_UCN,
  HR_UAB,
  HR_UBC,
  HR_UCA,
  HR_IA,
  HR_IB,
  HR_IC,
  HR_P_TOTAL,
  HR_PF_TOTAL,
  HR_IN_CALCULATED,
  HR_KWH_TOTAL,
  HR_IA_THD,
  HR_IB_THD,
  HR_IC_THD,
  HR_S_TOTAL
};

static constexpr uint16_t PARAMETERS_SIZE = 17;
float holdingRegisterValues[PARAMETERS_SIZE] = {};

#endif