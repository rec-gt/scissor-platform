#ifndef SubSystem_H
#define SubSystem_H

#include "../core/Globals.h"
#include "./SubGlobals.h"
#include "./SubRS485.h"
#include "./Sensor.h"

SubRS485 subRS485;

uint16_t temp = 0;
uint16_t moisture = 0;

Sensor s1(A0);
Sensor s2(A1);
Sensor s3(A2);
Sensor s4(A3);
Sensor s5(A4);
Sensor s6(A5);
Sensor s7(A6);
Sensor s8(A7);
Sensor s9(A8);
Sensor s10(A9);
Sensor s11(A10);
Sensor s12(A11);
Sensor s13(A12);
Sensor s14(A13);
Sensor s15(A14);
Sensor s16(A15);

Timer mbTimer;

class SubSystem {
public:
  SubSystem(void) {
    if (!mbRtuClient.begin(9600, SERIAL_8N1)) {
      Serial.println("Failed to start Modbus RTU Client!");
      while (1) {};
    }
  }

  void init() {
    configAnalogInputResolution(0);
  }

  void readModbusData() {
    if (mbTimer.autoTimeout(1000)) {
      if (!mbRtuClient.requestFrom(1, HOLDING_REGISTERS, 0, 2)) {
        Serial.print("Failed to read! ");
        Serial.println(mbRtuClient.lastError());
      } else {
        while (mbRtuClient.available()) {
          temp = mbRtuClient.read();
          moisture = mbRtuClient.read();
          Serial.println(temp);
          Serial.println(moisture);
        }
      }
    }
  }

  void buildAIPayload(
    uint16_t reading1,
    uint16_t reading2,
    uint16_t reading3,
    uint16_t reading4,
    uint16_t reading5,
    uint16_t reading6,
    uint16_t reading7,
    uint16_t reading8,
    uint16_t reading9,
    uint16_t reading10,
    uint16_t reading11,
    uint16_t reading12,
    uint16_t reading13,
    uint16_t reading14,
    uint16_t reading15,
    uint16_t reading16,
    uint16_t temp,
    uint16_t moisture) {
    AIPayload = F("[");
    AIPayload += reading1;
    AIPayload += F(",");
    AIPayload += reading2;
    AIPayload += F(",");
    AIPayload += reading3;
    AIPayload += F(",");
    AIPayload += reading4;
    AIPayload += F(",");
    AIPayload += reading5;
    AIPayload += F(",");
    AIPayload += reading6;
    AIPayload += F(",");
    AIPayload += reading7;
    AIPayload += F(",");
    AIPayload += reading8;
    AIPayload += F(",");
    AIPayload += reading9;
    AIPayload += F(",");
    AIPayload += reading10;
    AIPayload += F(",");
    AIPayload += reading11;
    AIPayload += F(",");
    AIPayload += reading12;
    AIPayload += F(",");
    AIPayload += reading13;
    AIPayload += F(",");
    AIPayload += reading14;
    AIPayload += F(",");
    AIPayload += reading15;
    AIPayload += F(",");
    AIPayload += reading16;
    AIPayload += F(",");
    AIPayload += temp;
    AIPayload += F(",");
    AIPayload += moisture;
    AIPayload += F("]");
  }

  void loop() {
    this->readModbusData();

    s1.loop();
    s2.loop();
    s3.loop();
    s4.loop();
    s5.loop();
    s6.loop();
    s7.loop();
    s8.loop();
    s9.loop();
    s10.loop();
    s11.loop();
    s12.loop();
    s13.loop();
    s14.loop();
    s15.loop();
    s16.loop();

    uint16_t reading1 = 1023 - constrain(s1.reading, 0, 1023);
    uint16_t reading2 = 1023 - constrain(s2.reading, 0, 1023);
    uint16_t reading3 = 1023 - constrain(s3.reading, 0, 1023);
    uint16_t reading4 = 1023 - constrain(s4.reading, 0, 1023);
    uint16_t reading5 = 1023 - constrain(s5.reading, 0, 1023);
    uint16_t reading6 = 1023 - constrain(s6.reading, 0, 1023);
    uint16_t reading7 = 1023 - constrain(s7.reading, 0, 1023);
    uint16_t reading8 = 1023 - constrain(s8.reading, 0, 1023);
    uint16_t reading9 = 1023 - constrain(s9.reading, 0, 1023);
    uint16_t reading10 = 1023 - constrain(s10.reading, 0, 1023);
    uint16_t reading11 = 1023 - constrain(s11.reading, 0, 1023);
    uint16_t reading12 = 1023 - constrain(s12.reading, 0, 1023);
    uint16_t reading13 = 1023 - constrain(s13.reading, 0, 1023);
    uint16_t reading14 = 1023 - constrain(s14.reading, 0, 1023);
    uint16_t reading15 = 1023 - constrain(s15.reading, 0, 1023);
    uint16_t reading16 = 1023 - constrain(s16.reading, 0, 1023);

    /*=== Change Display Value ===*/
    analogInputs[0].value = reading1;
    analogInputs[1].value = reading2;
    analogInputs[2].value = reading3;
    analogInputs[3].value = reading4;
    analogInputs[4].value = reading5;
    analogInputs[5].value = reading6;
    analogInputs[6].value = reading7;
    analogInputs[7].value = reading8;
    analogInputs[8].value = reading9;
    analogInputs[9].value = reading10;
    analogInputs[10].value = reading11;
    analogInputs[11].value = reading12;
    analogOutputs[0].value = reading13;
    analogOutputs[1].value = reading14;
    analogOutputs[2].value = reading15;
    analogOutputs[3].value = reading16;

    /*=== Change IoT Payload ===*/
    mainSystem.buildPayloads();
    this->buildAIPayload(reading1,
                         reading2,
                         reading3,
                         reading4,
                         reading5,
                         reading6,
                         reading7,
                         reading8,
                         reading9,
                         reading10,
                         reading11,
                         reading12,
                         reading13,
                         reading14,
                         reading15,
                         reading16,
                         temp,
                         moisture);
    iot.buildMsg(DIPayload, DOPayload, AIPayload, AOPayload);
  }

  ~SubSystem() {}
};

#endif