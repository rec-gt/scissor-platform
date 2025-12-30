#include <ArduinoRS485.h>
#include <ArduinoModbus.h>
#include "Globals.h"
#include "IFCUSlave.h"

IFCUSlave ifcuSlave0(0);
IFCUSlave ifcuSlave1(1);
IFCUSlave ifcuSlave2(2);

void setup() {
  Serial.begin(9600);

  /*=== String / Heap Memory Management ===*/
  cmpStr.reserve(32);
  bool remainStrRes = rs485SerialRecv.reserve(128);
  Serial.print(remainStrRes ? F("[Str Space OK]") : F("[String Space NOT OK]"));

  /*=== Modbus ===*/
  if (!mbClientSlave.begin(9600)) {
    Serial.println("Failed to start Slave's Modbus RTU Client!");
    while (1) {};
  }

  delay(1000);
}

void loop() {
  ifcuSlave0.write();
  delay(1000);
}