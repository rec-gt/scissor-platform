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

#include "DigitalOutput.h"
#include "DigitalInput.h"

DigitalInput digitalInput1(DI_PIN_1);
DigitalInput digitalInput2(DI_PIN_2);
DigitalInput digitalInput3(DI_PIN_3);
DigitalInput digitalInput4(DI_PIN_4);
DigitalInput digitalInput5(DI_PIN_5);
DigitalInput digitalInput6(DI_PIN_6);
DigitalInput digitalInput7(DI_PIN_7);
DigitalInput digitalInput8(DI_PIN_8);

DigitalOutput digitalOutput1(DO_PIN_1);
DigitalOutput digitalOutput2(DO_PIN_2);
DigitalOutput digitalOutput3(DO_PIN_3);
DigitalOutput digitalOutput4(DO_PIN_4);
DigitalOutput digitalOutput5(DO_PIN_5);
DigitalOutput digitalOutput6(DO_PIN_6);
DigitalOutput digitalOutput7(DO_PIN_7);
DigitalOutput digitalOutput8(DO_PIN_8);

AnalogInput analogInput1(AI_PIN_1);
AnalogInput analogInput2(AI_PIN_2);
AnalogInput analogInput3(AI_PIN_3);
AnalogInput analogInput4(AI_PIN_4);
AnalogInput analogInput5(AI_PIN_5);
AnalogInput analogInput6(AI_PIN_6);
AnalogInput analogInput7(AI_PIN_7);
AnalogInput analogInput8(AI_PIN_8);
AnalogInput analogInput9(AI_PIN_9);
AnalogInput analogInput10(AI_PIN_10);
AnalogInput analogInput11(AI_PIN_11);
AnalogInput analogInput12(AI_PIN_12);

void setup() {
  Serial.begin(9600);

  pinMode(AO_PIN_1, OUTPUT);
  pinMode(AO_PIN_2, OUTPUT);
  pinMode(AO_PIN_3, OUTPUT);
  pinMode(AO_PIN_4, OUTPUT);
}

void loop() {
}
