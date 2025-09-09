#include <ModbusMaster.h>

#ifndef Modbus485_h
#define Modbus485_h

#define RE_DE_PIN 22

ModbusMaster node;

void preTransmission() {
  digitalWrite(RE_DE_PIN, HIGH);  // 切換到發送模式
}

void postTransmission() {
  digitalWrite(RE_DE_PIN, LOW);  // 切換到接收模式
}

class Modbus485 {
private:
public:
  Modbus485(){};

  void setup() {
    pinMode(RE_DE_PIN, OUTPUT);
    digitalWrite(RE_DE_PIN, LOW);

    Serial3.begin(9600);
    node.begin(31, Serial3);
    node.preTransmission(preTransmission);
    node.postTransmission(postTransmission);
  };

  void handleRead3x(unsigned int addr, unsigned int value) {
    uint8_t result;
    uint16_t data;

    result = node.readHoldingRegisters(0x0001, 1);

    if (result == node.ku8MBSuccess) {
      data = node.getResponseBuffer(0);
      Serial.print("Data: ");
      Serial.println(data);
    } else {
      Serial.println("Read error");
    }
  }

  ~Modbus485(){};
};

extern Modbus485 modbus485;

#endif
