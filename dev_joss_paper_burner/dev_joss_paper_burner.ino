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

#define DI_NUMS 8
#define DO_NUMS 8
#define AI_NUMS 12
#define AO_NUMS 4

#include "Globals.h"
#include "DigitalInput.h"
#include "DigitalOutput.h"
#include "AnalogInput.h"
#include "AnalogOutput.h"
#include "MainSystem.h"
#include "NBIoT.h"

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

AnalogInput analogInputs[AI_NUMS] = {
  AnalogInput(AI_PIN_1, AI_MAPPING_MODE_0_10V),
  AnalogInput(AI_PIN_2, AI_MAPPING_MODE_0_10V),
  AnalogInput(AI_PIN_3, AI_MAPPING_MODE_0_10V),
  AnalogInput(AI_PIN_4, AI_MAPPING_MODE_0_10V),
  AnalogInput(AI_PIN_5, AI_MAPPING_MODE_0_10V),
  AnalogInput(AI_PIN_6, AI_MAPPING_MODE_0_10V),
  AnalogInput(AI_PIN_7, AI_MAPPING_MODE_0_10V),
  AnalogInput(AI_PIN_8, AI_MAPPING_MODE_0_10V),
  AnalogInput(AI_PIN_9, AI_MAPPING_MODE_0_10V),
  AnalogInput(AI_PIN_10, AI_MAPPING_MODE_0_10V),
  AnalogInput(AI_PIN_11, AI_MAPPING_MODE_0_10V),
  AnalogInput(AI_PIN_12, AI_MAPPING_MODE_0_10V),
};

MainSystem mainSystem(digitalInputs, digitalOutputs, analogInputs, analogOutputs);

NBIoT nbiot;

void setup() {
  Serial.begin(9600);
  NBIoTSerial.begin(9600);
  nbiot.debug();
  nbiot.init(true);
  // analogReference(EXTERNAL);
}

void loop() {
  /*=== Register NBIoT ===*/
  nbiot.loop();

  /*=== Listen Inputs ===*/
  mainSystem.listen();

  /*=== Prepare for NBIoT Publish ===*/
  mainSystem.preparePubMsg();

  /*=== NBIoT Subscribe (for DO & AO only)===*/
  mainSystem.commandHook();

  /*=== Display (for NBIoT, DO, AO, DI, AI)===*/
  // mainSystem.handleDisplayMsg();


  delay(500);
}
