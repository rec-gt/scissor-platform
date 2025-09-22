#define DI_PIN_1 32
#define DI_PIN_2 33
#define DI_PIN_3 34
#define DI_PIN_4 35
#define DI_PIN_5 36
#define DI_PIN_6 37
#define DI_PIN_7 38
#define DI_PIN_8 39

#define DO_PIN_1 40
#define DO_PIN_2 41
#define DO_PIN_3 42
#define DO_PIN_4 43
#define DO_PIN_5 44
#define DO_PIN_6 45
#define DO_PIN_7 46
#define DO_PIN_8 47

#define AI_PIN_1 A0
#define AI_PIN_2 A1
#define AI_PIN_3 A2
#define AI_PIN_4 A3
#define AI_PIN_5 A4
#define AI_PIN_6 A5
#define AI_PIN_7 A6
#define AI_PIN_8 A7
#define AI_PIN_9 A8
#define AI_PIN_10 A9
#define AI_PIN_11 A10
#define AI_PIN_12 A11

#define AO_PIN_1 4
#define AO_PIN_2 5
#define AO_PIN_3 6
#define AO_PIN_4 7

// TODO: change to these pins later
// #define AO_PIN_1 2
// #define AO_PIN_2 3
// #define AO_PIN_3 5
// #define AO_PIN_4 6

#define DRY_CONTACT_PIN_1 27
#define DRY_CONTACT_PIN_2 28
#define DRY_CONTACT_PIN_3 29
#define DRY_CONTACT_PIN_4 30
#define DRY_CONTACT_PIN_5 31

#define DI_NUMS 8
#define DO_NUMS 8
#define AI_NUMS 12
#define AO_NUMS 4
#define DRY_CONTACT_NUMS 5

#include "DigitalInput.h"
#include "DigitalOutput.h"
#include "AnalogInput.h"
#include "AnalogOutput.h"
#include "DryContact.h"
#include "MainSystem.h"
#include "NBIoT.h"
#include "DisplayClient.h"
#include "Modbus485.h"
#include "Test.h"
#include "Utils.h"

DryContact dryContact1(DRY_CONTACT_PIN_1);

DigitalInput digitalInputs[DI_NUMS] = {
  DigitalInput(DI_PIN_1),
  DigitalInput(DI_PIN_2),
  DigitalInput(DI_PIN_3),
  DigitalInput(DI_PIN_4),
  DigitalInput(DI_PIN_5),
  DigitalInput(DI_PIN_6),
  DigitalInput(DI_PIN_7),
  DigitalInput(DI_PIN_8),
};

DigitalOutput digitalOutputs[DO_NUMS]{
  DigitalOutput(DO_PIN_1),
  DigitalOutput(DO_PIN_2),
  DigitalOutput(DO_PIN_3),
  DigitalOutput(DO_PIN_4),
  DigitalOutput(DO_PIN_5),
  DigitalOutput(DO_PIN_6),
  DigitalOutput(DO_PIN_7),
  DigitalOutput(DO_PIN_8),
};

AnalogOutput analogOutputs[AO_NUMS]{
  AnalogOutput(AO_PIN_1),
  AnalogOutput(AO_PIN_2),
  AnalogOutput(AO_PIN_3),
  AnalogOutput(AO_PIN_4),
};

byte aiMappingMode = dryContact1.isConnected() ? AI_MAPPING_MODE_4_20MA : AI_MAPPING_MODE_0_10V;

// TODO: change the config method


AnalogInput analogInputs[AI_NUMS] = {
  AnalogInputV(AI_PIN_1, AI_MAPPING_MODE_4_20MA),
  AnalogInputV(AI_PIN_2, AI_MAPPING_MODE_0_10V),
  AnalogInputV(AI_PIN_3, AI_MAPPING_MODE_4_20MA),
  AnalogInputV(AI_PIN_4, AI_MAPPING_MODE_0_10V),
  AnalogInputV(AI_PIN_5, AI_MAPPING_MODE_4_20MA),
  AnalogInputV(AI_PIN_6, AI_MAPPING_MODE_0_10V),
  AnalogInputV(AI_PIN_7, AI_MAPPING_MODE_4_20MA),
  AnalogInputV(AI_PIN_8, AI_MAPPING_MODE_0_10V),
  AnalogInputV(AI_PIN_9, AI_MAPPING_MODE_4_20MA),
  AnalogInputV(AI_PIN_10, AI_MAPPING_MODE_0_10V),
  AnalogInputV(AI_PIN_11, AI_MAPPING_MODE_4_20MA),
  AnalogInputV(AI_PIN_12, AI_MAPPING_MODE_0_10V)
};

void getAnalogInputMode() {
  for (size_t i = 0; i < AI_NUMS; i++) {
    analogInputsMode |= (analogInputs[i].mappingMode == AI_MAPPING_MODE_4_20MA ? 1 : 0) << i;
  }
  Serial.print(F("AI MAPPING: "));
  Serial.println(analogInputsMode);
}

// AnalogInput analogInputsA[AI_NUMS] = {
//   AnalogInputA(AI_PIN_1, aiMappingMode, 1590, 3205, 4819, 6441, 8055),
//   AnalogInputA(AI_PIN_2, aiMappingMode, 1591, 3204, 4816, 6434, 8047),
//   AnalogInputA(AI_PIN_3, aiMappingMode, 1591, 3207, 4822, 6444, 8060),
//   AnalogInputA(AI_PIN_4, aiMappingMode, 0, 0, 0, 0, 0),
//   AnalogInputA(AI_PIN_5, aiMappingMode, 1590, 3204, 4816, 6436, 8049),
//   AnalogInputA(AI_PIN_6, aiMappingMode, 1589, 3200, 4811, 6429, 8041),
//   AnalogInputA(AI_PIN_7, aiMappingMode, 1594, 3211, 4827, 6451, 8068),
//   AnalogInputA(AI_PIN_8, aiMappingMode, 1594, 3210, 4825, 6447, 8062),
//   AnalogInputA(AI_PIN_9, aiMappingMode, 1587, 3198, 4808, 6426, 8037),
//   AnalogInputA(AI_PIN_10, aiMappingMode, 1589, 3200, 4811, 6428, 8039),
//   AnalogInputA(AI_PIN_11, aiMappingMode, 1587, 3199, 4810, 6428, 8039),
//   AnalogInputA(AI_PIN_12, aiMappingMode, 1595, 3211, 4825, 6447, 8063),
// };

// AnalogInput analogInputsV[AI_NUMS] = {
//   AnalogInputV(AI_PIN_1, aiMappingMode, 789, 1610, 4063, 8158, 16317),
//   AnalogInputV(AI_PIN_2, aiMappingMode, 791, 1611, 4061, 8150, 16302),
//   AnalogInputV(AI_PIN_3, aiMappingMode, 790, 1610, 4064, 8160, 16319),
//   AnalogInputV(AI_PIN_4, aiMappingMode, 0, 0, 0, 0, 0),
//   AnalogInputV(AI_PIN_5, aiMappingMode, 790, 1610, 4060, 8150, 16304),
//   AnalogInputV(AI_PIN_6, aiMappingMode, 789, 1607, 4055, 8142, 16292),
//   AnalogInputV(AI_PIN_7, aiMappingMode, 789, 1609, 4061, 8154, 16312),
//   AnalogInputV(AI_PIN_8, aiMappingMode, 793, 1614, 4069, 8165, 16325),
//   AnalogInputV(AI_PIN_9, aiMappingMode, 788, 1606, 4053, 8139, 16289),
//   AnalogInputV(AI_PIN_10, aiMappingMode, 789, 1608, 4056, 8142, 16292),
//   AnalogInputV(AI_PIN_11, aiMappingMode, 788, 1607, 4055, 8141, 16292),
//   AnalogInputV(AI_PIN_12, aiMappingMode, 793, 1615, 4069, 8164, 16323),
// };

// MainSystem mainSystem(digitalInputs, digitalOutputs, aiMappingMode == 0 ? analogInputsA : analogInputsV, analogOutputs, aiMappingMode);

MainSystem mainSystem(digitalInputs, digitalOutputs, analogInputs, analogOutputs, aiMappingMode);

NBIoT nbiot;

DisplayClient displayClient;

Modbus485 modbus485;

Test test;

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
  nbiotPubMsgCommand.reserve(256);
  bool res = rubbishStr.reserve(278);  //push it to limit

  Serial.print(res ? F("[Str Space OK]") : F("[String Space NOT OK]"));

  /*=== NBIoT ===*/
  nbiot.init(true);
  nbiot.debug();

  /*=== Display ===*/
  displayClient.setup();

  /*=== AI mapping mode===*/
  getAnalogInputMode();
}

void loop() {
  /*=== Register NBIoT ===*/
  nbiot.loop();

  /*=== Register Modbus ===*/
  modbus485.loop();

  /*=== Register MainSystem ===*/
  mainSystem.loop();
  // analogOutputs[0].set(255);
  // analogOutputs[1].set(255);
  // analogOutputs[2].set(255);
  // analogOutputs[3].set(255);
  /*=== Register Outputs Testing ===*/
  // test.DO(digitalOutputs);
  // test.AO(analogOutputs);

  delay(10);
}