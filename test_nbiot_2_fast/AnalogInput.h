#ifndef AnalogInput_H
#define AnalogInput_H

#define AI_MAPPING_MODE_4_20MA 0
#define AI_MAPPING_MODE_0_10V 1

class AnalogInput {
private:
  byte pin;

public:
  uint16_t reading;
  uint16_t smoothedReading;
  uint16_t value;
  byte mappingMode;

  AnalogInput() {}

  AnalogInput(byte pin, byte mappingMode = AI_MAPPING_MODE_4_20MA)
    : pin(pin), mappingMode(mappingMode) {
    pinMode(pin, INPUT);
  }

  void listen() {
    uint32_t avg = 0;
    for (size_t i = 0; i < 32; i++) {
      avg += analogRead(this->pin);
    };
    this->value = avg / 32;
  }

  uint16_t getValue() {
    return this->value;
  }
};

#endif