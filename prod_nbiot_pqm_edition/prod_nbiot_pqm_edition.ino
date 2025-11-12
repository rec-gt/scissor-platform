#include "./core/MainSystem.h"
#include "./core/NBIoT.h"
#include "./core/DisplayClient.h"
#include "./core/Utils.h"
#include "./core/AsyncTimer.h"
#include "./edition/SubSystem.h"
#include <avr/wdt.h>

MainSystem mainSystem;

NBIoT nbiot;

DisplayClient displayClient;

Utils utils;

SubSystem subSystem;

AsyncTimer systemTimer(86400000UL);

void setup() {
  Serial.begin(9600);
  analogReference(EXTERNAL);

  /*=== String / Heap Memory Management ===*/
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
  nbiotPubMsgPayload.reserve(128);
  bool remainStrRes = nbiotPubMsgCommand.reserve(128);
  Serial.print(remainStrRes ? F("[Str Space OK]") : F("[String Space NOT OK]"));

  /*=== NBIoT ===*/
  nbiot.init(true);

  /*=== Display ===*/
  displayClient.setup();

  /*=== SubSystem Init ===*/
  subSystem.init();

  /*=== Watchdog ===*/
  wdt_enable(WDTO_8S);
}

void loop() {
  /*=== Register NBIoT ===*/
  nbiot.loop();

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