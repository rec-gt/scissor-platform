#include "./core/MainSystem.h"
#include "./core/IoT.h"
#include "./core/DisplayClient.h"
#include "./core/Utils.h"
#include "./core/AsyncTimer.h"
#include "./edition/SubSystem.h"
#include <avr/wdt.h>

MainSystem mainSystem;

IoT iot;

DisplayClient displayClient;

Utils utils;

SubSystem subSystem;

AsyncTimer systemTimer(86400000UL);

void setup() {
  Serial.begin(9600);
  analogReference(EXTERNAL);

  /*=== String / Heap Memory Management ===*/
  iotCSQ.reserve(8);
  iotCGATT.reserve(8);
  iotCEREG.reserve(8);
  iotPubAck.reserve(8);
  iotSubAck.reserve(8);
  iotSubMsgContent.reserve(8);
  iotIMEI.reserve(32);
  cmpStr.reserve(32);
  AOPayload.reserve(64);
  iotConnCmd.reserve(64);
  iotSubsCmd.reserve(64);
  iotPubMsgPrepare.reserve(64);
  AIPayload.reserve(128);
  rs485SerialRecv.reserve(128);
  iotSerialRecv.reserve(128);
  iotPubMsgPayload.reserve(256);
  bool remainStrRes = iotPubMsgCommand.reserve(512);
  Serial.print(remainStrRes ? F("[Str Space OK]") : F("[String Space NOT OK]"));

  /*=== IoT ===*/
  iot.init(true);
  iot.debug();

  /*=== Display ===*/
  displayClient.setup();

  /*=== SubSystem Init ===*/
  subSystem.init();

  /*=== Watchdog ===*/
  wdt_enable(WDTO_8S);
}

void loop() {
  /*=== Register IoT ===*/
  iot.loop();

  /*=== Register MainSystem ===*/
  mainSystem.loop();

  /*=== Register display ===*/
  displayClient.loop();

  /*=== Register subSystem ===*/
  subSystem.loop();

  /*=== Pet the dog ===*/
  if (!systemTimer.isExpired()) {
    wdt_reset();
  }

  delay(10);
}