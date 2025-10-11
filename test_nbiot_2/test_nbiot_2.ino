

#include "DigitalInput.h"
#include "DigitalOutput.h"
#include "AnalogInput.h"
#include "AnalogOutput.h"
#include "DryContact.h"
#include "MainSystem.h"
#include "NBIoT.h"
#include "DisplayClient.h"
#include "Modbus485.h"
#include "Utils.h"
#include "Globals.h"

MainSystem mainSystem;
NBIoT nbiot;
DisplayClient displayClient;
Modbus485 modbus485;
Utils utils;

void setup() {
  Serial.begin(9600);
  analogReference(EXTERNAL);

  /*=== String Management ===*/
  nbiotCSQ.reserve(8);
  nbiotCGATT.reserve(8);
  nbiotCEREG.reserve(8);
  nbiotPubAck.reserve(8);
  nbiotSubAck.reserve(8);
  nbiotSubMsgContent.reserve(8);
  nbiotIMEI.reserve(32);
  cmpStr.reserve(32);
  AOPayload.reserve(64);
  nbIotConnCmd.reserve(64);
  nbiotSubsCmd.reserve(64);
  nbiotPubMsgPrepare.reserve(64);
  AIPayload.reserve(128);
  rs485SerialRecv.reserve(128);
  nbiotSerialRecv.reserve(128);
  nbiotPubMsgPayload.reserve(256);
  nbiotPubMsgCommand.reserve(512);
  bool rubbishStrRes = rubbishStr.reserve(1024);  //push it to limit
  Serial.print(rubbishStrRes ? F("[Str Space OK]") : F("[String Space NOT OK]"));

  /*=== NBIoT ===*/
  nbiot.init(true);
  // nbiot.debug();

  /*=== Display ===*/
  displayClient.setup();
}

void loop() {
  /*=== Register NBIoT ===*/
  nbiot.loop();

  /*=== Register Modbus ===*/
  modbus485.loop();

  /*=== Register MainSystem ===*/
  mainSystem.loop();

  delay(10);
}