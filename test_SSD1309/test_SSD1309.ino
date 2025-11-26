#include <ModbusMaster.h>
#define RXD2 16
#define TXD2 17
// instantiate ModbusMaster object
ModbusMaster node;
uint8_t result;
uint16_t data[16];


void setup() {
  Serial.begin(115200);
  Serial2.begin(9600, SERIAL_8N1, RXD2, TXD2);
  node.begin(31, Serial2);
}


void loop() {
  result = node.readInputRegisters(30000, 13);
  if (result == node.ku8MBSuccess) {
    for (int i = 0; i < 12; i++) {
      Serial.println(node.getResponseBuffer(i));
    }
  } else {
    Serial.println("Cannot Fetch Data");
  }
  delay(1000);
}