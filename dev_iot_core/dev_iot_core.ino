#include "./core/MainSystem.h"
#include "./core/IoT.h"
#include "./core/DisplayClient.h"
#include "./core/Utils.h"
#include "./core/Timer.h"
#include "./edition/SubSystem.h"
#include <avr/wdt.h>

MainSystem mainSystem;

IoT iot;

DisplayClient displayClient;

Utils utils;

SubSystem subSystem;

Timer systemTimer(86400000);

void setup() {
  Serial.begin(9600);
  analogReference(EXTERNAL);

  /*=== String / Heap Memory Management ===*/
  iotCSQ.reserve(8);
  iotCGATT.reserve(8);
  iotCEREG.reserve(8);
  mqttSubsMsgContent.reserve(8);
  iotIMEI.reserve(32);
  iotExtractedRecv.reserve(32);
  AOPayload.reserve(64);
  mqttConnCmd.reserve(64);
  mqttSubsCmd.reserve(64);
  mqttPublMsgPrepare.reserve(128);
  AIPayload.reserve(128);
  rs485SerialRecv.reserve(128);
  iotSerialRecv.reserve(128);
  serialInputCmd.reserve(128);
  mqttPublMsgPayload.reserve(256);
  bool remainStrRes = debugStr.reserve(512);
  Serial.print(remainStrRes ? F("[Str Space OK]") : F("[String Space NOT OK]"));

  /*=== IoT ===*/
  iot.init();

  /*=== Display ===*/
  displayClient.setup();

  /*=== SubSystem Init ===*/
  subSystem.init();

  /*=== Watchdog ===*/
  // wdt_enable(WDTO_8S);
}

void loop() {
  utils.serialInput();

  /*=== Register IoT ===*/
  iot.loop();

  /*=== Register MainSystem ===*/
  mainSystem.loop();

  /*=== Register display ===*/
  displayClient.loop();

  /*=== Register subSystem ===*/
  subSystem.loop();

  /*=== Pet the dog ===*/
  // if (!systemTimer.isExpired()) {
  //   wdt_reset();
  // }

  delay(10);
}