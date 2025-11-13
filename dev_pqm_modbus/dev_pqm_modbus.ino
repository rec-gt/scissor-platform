#include <ArduinoRS485.h>
#include <ArduinoModbus.h>
#include "PowerMeter.h"
#include "Globals.h"
#include "NBIoT.h"

PowerMeter powerMeter(100);
NBIoT nbiot;

void setup() {
  Serial.begin(9600);

  /*=== String / Heap Memory Management ===*/
  nbiotCSQ.reserve(8);
  nbiotCGATT.reserve(8);
  nbiotCEREG.reserve(8);
  nbiotPubAck.reserve(8);
  nbiotSubAck.reserve(8);
  nbiotSubMsgContent.reserve(8);
  nbiotIMEI.reserve(32);
  cmpStr.reserve(32);
  nbIotConnCmd.reserve(64);
  nbiotSubsCmd.reserve(64);
  nbiotPubMsgPrepare.reserve(64);
  rs485SerialRecv.reserve(128);
  nbiotSerialRecv.reserve(128);
  nbiotPubMsgPayload.reserve(256);
  bool remainStrRes = nbiotPubMsgCommand.reserve(256);
  Serial.print(remainStrRes ? F("[Str Space OK]") : F("[String Space NOT OK]"));

  /*=== Modbus ===*/
  if (!mbClient.begin(9600)) {
    Serial.println("Failed to start Modbus RTU Client!");
    while (1) {};
  }

  nbiot.init(true);

  delay(1000);
}

void loop() {
  nbiot.loop();

  powerMeter.loop();

  delay(10);
}