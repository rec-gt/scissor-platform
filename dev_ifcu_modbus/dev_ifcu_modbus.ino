#include <ArduinoRS485.h>
#include <ArduinoModbus.h>

RS485Class rs485(Serial1, 2, 3, 4);
ModbusRTUClientClass mbClient(rs485);

void handleWrite4x(int slaveId, int addr, int value) {
  if (!mbClient.holdingRegisterWrite(slaveId, addr, value)) {
    Serial.println(mbClient.lastError());
  } else {
    Serial.println("Response OK");
  }
}

void setup() {
  Serial.begin(9600);

  // Start Serial1 as the Modbus RTU communication port
  if (!mbClient.begin(9600)) {
    Serial.println("Failed to start Modbus RTU Client!");
    while (1) {};
  }

  handleWrite4x(31, 40003, 0);
  handleWrite4x(31, 40005, 2225);
}

void loop() {
}