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
    uint16_t reading12) {
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
    AIPayload += F("]");
  }

  void buildAOPayload(
    uint16_t reading1,
    uint16_t reading2,
    uint16_t reading3,
    uint16_t reading4) {
    AOPayload = F("[");
    AOPayload += reading1;
    AOPayload += F(",");
    AOPayload += reading2;
    AOPayload += F(",");
    AOPayload += reading3;
    AOPayload += F(",");
    AOPayload += reading4;
    AOPayload += F("]");
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

    /*=== Change Display Value ===*/
    analogInputs[0].value = s1.calibratedReading;
    analogInputs[1].value = s2.calibratedReading;
    analogInputs[2].value = s3.calibratedReading;
    analogInputs[3].value = s4.calibratedReading;
    analogInputs[4].value = s5.calibratedReading;
    analogInputs[5].value = s6.calibratedReading;
    analogInputs[6].value = s7.calibratedReading;
    analogInputs[7].value = s8.calibratedReading;
    analogInputs[8].value = s9.calibratedReading;
    analogInputs[9].value = s10.calibratedReading;
    analogInputs[10].value = s11.calibratedReading;
    analogInputs[11].value = s12.calibratedReading;
    analogOutputs[0].value = s13.calibratedReading;
    analogOutputs[1].value = s14.calibratedReading;
    analogOutputs[2].value = temp / 10;
    analogOutputs[3].value = moisture / 10;

    /*=== Change IoT Payload ===*/
    mainSystem.buildPayloads();
    this->buildAIPayload(s1.calibratedReading,
                         s2.calibratedReading,
                         s3.calibratedReading,
                         s4.calibratedReading,
                         s5.calibratedReading,
                         s6.calibratedReading,
                         s7.calibratedReading,
                         s8.calibratedReading,
                         s9.calibratedReading,
                         s10.calibratedReading,
                         s11.calibratedReading,
                         s12.calibratedReading);

    this->buildAOPayload(s13.calibratedReading,
                         s14.calibratedReading,
                         temp / 10,
                         moisture / 10);

    iot.buildMsg(DIPayload, DOPayload, AIPayload, AOPayload);
  }

  ~SubSystem() {}
};

#endif