#include "./core/MainSystem.h"
#include "./core/IoT.h"
#include "./core/DisplayClient.h"
#include "./core/Utils.h"
#include "./core/Timer.h"
#include "./edition/SubSystem.h"
#include <avr/wdt.h>
#include <HardwareSerial.h>

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
  iotModel.reserve(16);
  iotIMEI.reserve(16);
  AOPayload.reserve(32);
  serialInputCmd.reserve(32);
  mqttConnCmd.reserve(64);
  mqttSubsCmd.reserve(64);
  mqttPublMsgPrepare.reserve(64);
  AIPayload.reserve(64);
  rs485SerialRecv.reserve(128);
  mqttPublMsgPayload.reserve(128);
  bool remainStrRes = iotSerialRecv.reserve(512);
  // bool remainStrRes = debugStr.reserve(257);
  Serial.println(remainStrRes ? F("[Str Space OK]") : F("[String Space NOT OK]"));

  /*=== IoT ===*/
  iot.init();

  /*=== Display ===*/
  displayClient.setup();

  /*=== SubSystem Init ===*/
  subSystem.init();

  /*=== Watchdog ===*/
  wdt_enable(WDTO_8S);
}

void loop() {
  utils.serialInput();

  /*=== Register MainSystem ===*/
  mainSystem.loop();

  /*=== Register subSystem ===*/
  subSystem.loop();

  /*=== Register display ===*/
  displayClient.loop();

  /*=== Register IoT ===*/
  iot.loop();

  /*=== Pet the dog ===*/
  if (!systemTimer.isExpired()) {
    wdt_reset();
  }

  delay(10);

  // Serial.println(analogInputs[0].value);
}

// TODO: EEPROM -AO -DO (Optional)
// TODO: Event Trigger -AO -DO