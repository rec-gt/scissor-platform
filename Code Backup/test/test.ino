//醬是創客 開發實作的好夥伴
#include <WiFi.h>
#include <ModbusMaster.h>
#define RXD2 16
#define TXD2 17
// instantiate ModbusMaster object
ModbusMaster node;
uint8_t result;
uint16_t data[2];
 
 
void setup()
{
  Serial.begin(115200);
  Serial2.begin(9600, SERIAL_8N1, RXD2, TXD2);
  // Modbus slave ID 1 使用 Serial2
  node.begin(1, Serial2);
}
 
 
void loop()
{
  //InputRegister 0x0001~0x0002
  result = node.readInputRegisters(0x0001, 2);
   
  if (result == node.ku8MBSuccess)
  {
    //InputRegister 0x0001
    data[0] = node.getResponseBuffer(0);
    Serial.println("Temperature");
    Serial.println(data[0] * 0.1);
    //InputRegister 0x0002
    data[1] = node.getResponseBuffer(1);
    Serial.println("Humidity");
    Serial.println(data[1] * 0.1);
  }
  delay(10000);
}