#ifndef ModbusRTU_H
#define ModbusRTU_H
#define INPUT_REGISTER_COUNT 7

class ModbusRTUClient {
private:
  uint16_t data[INPUT_REGISTER_COUNT];

public:
  ModbusRTUClient(){};

  void readData() {
    uint8_t result;
    result = node.readInputRegisters(0x0000, INPUT_REGISTER_COUNT);

    switch (result) {
      case node.ku8MBSuccess:
        for (uint8_t i = 0; i < INPUT_REGISTER_COUNT; i++) {
          data[i] = node.getResponseBuffer(i);
          Serial.print("Register ");
          Serial.print(i);
          Serial.print(": ");
          Serial.println(data[i]);
        }
        break;

      case node.ku8MBIllegalFunction:
        Serial.println("Illegal function");
        break;
      case node.ku8MBIllegalDataAddress:
        Serial.println("Illegal data address");
        break;
      case node.ku8MBIllegalDataValue:
        Serial.println("Illegal data value");
        break;
      case node.ku8MBSlaveDeviceFailure:
        Serial.println("Slave device failure");
        break;
      case node.ku8MBInvalidSlaveID:
        Serial.println("Invalid slave ID");
        break;
      case node.ku8MBInvalidFunction:
        Serial.println("Invalid function");
        break;
      case node.ku8MBResponseTimedOut:
        Serial.println("Response timed out");
        break;
      case node.ku8MBInvalidCRC:
        Serial.println("Invalid CRC");
        break;
      default:
        Serial.println("Unknown error");
        break;
    }
  }

  ~ModbusRTUClient(){};
};

#endif