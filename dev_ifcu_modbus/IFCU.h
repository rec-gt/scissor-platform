#ifndef IFCU_H
#define IFCU_H

RS485Class rs485(Serial1, 2, 3, 4);
ModbusRTUClientClass mbClient(rs485);

class IFCU {
private:
  byte slaveId;
  
  void handleWrite4x(int slaveId, int addr, int value) {
    if (!mbClient.holdingRegisterWrite(slaveId, addr, value)) {
      Serial.println(mbClient.lastError());
    } else {
      Serial.println("Response OK");
    }
  }

public:
  void IFCU(int slaveId)
    : slaveId(slaveId);

  void on() {
  }
};

#endif
