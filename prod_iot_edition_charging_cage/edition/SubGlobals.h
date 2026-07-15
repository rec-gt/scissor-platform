#ifndef SUB_GLOBALS_H
#define SUB_GLOBALS_H

#include <ArduinoRS485.h>
#include <ArduinoModbus.h>
#include "../core/Globals.h"
#include "../core/Toggle.h"

#define RS485Serial Serial3
#define RS485_RE_DE_PIN 22

/*=== For Controller ===*/

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

/*=== For Subsystem ===*/
#define PARAMETERS_SIZE 16
uint16_t holdingRegisterValues[PARAMETERS_SIZE] = {};

enum SUBSYS_STATUS {
  SUBSYS_RUNNING,
  SUBSYS_STOPPED,
};

enum SUBSYS_HEALTH {
  SUBSYS_HEALTHY,
  SUBSYS_FAILURE,
};

class SysMonitor {
private:
  byte sysStatus;
  byte sysHealth;
public:
  SysMonitor(void) {
    this->sysStatus = SUBSYS_RUNNING;
    this->sysHealth = SUBSYS_HEALTHY;
  };

  void setStatus(byte status) {
    this->sysStatus = status;
  }

  bool isStatus(byte status) {
    return this->sysStatus == status;
  }

  void setHealth(byte health) {
    this->sysHealth = health;
  }

  bool isHealth(byte health) {
    return this->sysHealth == health;
  }
};

/*=== Charging Cage Config ===*/
#define TEMPERATURE_CHANNEL_SIZE 13  // 10個籠 + 1個環境溫度 
uint16_t THRESHOLD_DANGEROUS = 600;  // 60°C
uint16_t THRESHOLD_SAFE = THRESHOLD_DANGEROUS - 20;

extern SysMonitor sysMonitor;
#endif