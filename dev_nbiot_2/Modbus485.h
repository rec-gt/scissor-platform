#include <ArduinoRS485.h>
#include <ArduinoModbus.h>

#ifndef Modbus485_h
#define Modbus485_h

RS485Class rs485(Serial3, 14, 15, 22);
ModbusRTUClientClass modbusClient(rs485);

class Modbus485 {
private:

public:
  Modbus485(){};

  void setup() {
    if (!modbusClient.begin(9600)) {
      Serial.println("Failed to start Modbus RTU Client!");
      while (1) {};
    }
  };

  ~Modbus485(){};
};

extern Modbus485 modbus485;

#endif
