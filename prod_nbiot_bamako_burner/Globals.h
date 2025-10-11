#ifndef GLOBALS_H
#define GLOBALS_H

/*=== Main System ===*/
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

AnalogInputFaster analogInputs[AI_NUMS] = {
  AnalogInputFaster(AI_PIN_1),
  AnalogInputFaster(AI_PIN_2),
  AnalogInputFaster(AI_PIN_3),
  AnalogInputFaster(AI_PIN_4),
  AnalogInputFaster(AI_PIN_5),
  AnalogInputFaster(AI_PIN_6),
  AnalogInputFaster(AI_PIN_7),
  AnalogInputFaster(AI_PIN_8),
  AnalogInputFaster(AI_PIN_9),
  AnalogInputFaster(AI_PIN_10),
  AnalogInputFaster(AI_PIN_11),
  AnalogInputFaster(AI_PIN_12)
};

DryContact dryContacts[DRY_CONTACT_NUMS] = {
  DryContact(DRY_CONTACT_PIN_1),
  DryContact(DRY_CONTACT_PIN_2),
  DryContact(DRY_CONTACT_PIN_3),
  DryContact(DRY_CONTACT_PIN_4),
  DryContact(DRY_CONTACT_PIN_5),
};

String AIPayload = "";
String AOPayload = "";
String cmpStr = "";

/*=== NBIoT ===*/
String nbiotSerialRecv = "";
String nbIotConnCmd = "";
String nbiotSubsCmd = "";

String nbiotCSQ = "";
String nbiotIMEI = "";
String nbiotCGATT = "";
String nbiotCEREG = "";

String nbiotPubMsgPayload = "";
String nbiotPubMsgPrepare = "";
String nbiotPubMsgCommand = "";

String nbiotSubMsgContent = "";

String nbiotPubAck = "";
String nbiotSubAck = "";

/*=== Modbus485 ===*/
String rs485SerialRecv = "";

String rubbishStr = "";

#endif