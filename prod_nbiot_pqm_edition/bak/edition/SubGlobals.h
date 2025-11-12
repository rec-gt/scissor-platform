#ifndef SUB_GLOBALS_H
#define SUB_GLOBALS_H

/*=== For Modbus ===*/
RS485Class rs485(Serial3, 0, 0, 22);
ModbusRTUClientClass mbClient(rs485);

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