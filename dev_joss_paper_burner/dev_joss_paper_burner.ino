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

#include "DigitalOutput.h"
#include "DigitalInput.h"
#include "AnalogInput.h"
#include "AnalogOutput.h"


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
  AnalogInput(AI_PIN_1),
  AnalogInput(AI_PIN_2),
  AnalogInput(AI_PIN_3),
  AnalogInput(AI_PIN_4),
  AnalogInput(AI_PIN_5),
  AnalogInput(AI_PIN_6),
  AnalogInput(AI_PIN_7),
  AnalogInput(AI_PIN_8),
  AnalogInput(AI_PIN_9),
  AnalogInput(AI_PIN_10),
  AnalogInput(AI_PIN_11),
  AnalogInput(AI_PIN_12),
};


void setup() {
  Serial.begin(9600);
  analogReference(EXTERNAL);
}

void loop() {
  for (size_t i = 0; i < AI_NUMS; i++) {
    analogInputs[i].listen();
  }

  for (size_t i = 0; i < AI_NUMS; i++) {
    Serial.print(i);
    Serial.print(": ");
    Serial.print(analogInputs[i].getValue());
    Serial.print(", ");
    // Serial.println(map(analogInputs[i].getValue(), 0, 16368, 0, 50000));// for 0-5V
    Serial.println(map(analogInputs[i].getValue(), 3360, 16368, 10000, 50000)); // for 4-20mA
  }

  delay(500);
}
