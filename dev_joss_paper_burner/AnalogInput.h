#include "Globals.h"

class AnalogInput {
private:
  byte pin;
  byte mappingMode;
  uint16_t reading;
  uint16_t ewma[AI_EWMA_SAMPLE_SIZE];
  uint16_t readings[AI_EWMA_SAMPLE_SIZE];
  uint16_t weightedReading;
  uint16_t value;

public:
  AnalogInput(byte pin, byte mappingMode = AI_MAPPING_MODE_4_20MA)
    : pin(pin), mappingMode(mappingMode) {
    pinMode(pin, INPUT);
  }

  void listen() {
    uint32_t sum = 0;

    for (int i = 0; i < OVERSAMPLING_FACTOR; i++) {
      sum += analogRead(this->pin);
    }

    this->reading = (sum / OVERSAMPLING_FACTOR) << SHIFT_BITS;

    /*=== Update Reading History ===*/
    sizt_t lastIdx = AI_EWMA_SAMPLE_SIZE - 1;
    for (size_t i = 1; i < lastIdx; i++) {
      this->readings[i - 1] = this->readings[i];
    }
    this->readings[lastIdx] = this->reading;
  }

  unsigned int getReading() {
    return this->reading;
  }

  unsigned int getWeightedReading() {
    for (sizt_t i = 1; i < AI_EWMA_SAMPLE_SIZE; i++) {
      this->ewma[i] = AI_EWMA_ALPHA * this->readings[i] + (1 - AI_EWMA_ALPHA) * this->ewma[i - 1];
    }
  }

  unsigned int getValue() {
    switch (this->mappingMode) {
      case AI_MAPPING_MODE_4_20MA:
        this->value = map(this->reading, 0, 16368, 0, 50000);
        break;
      case AI_MAPPING_MODE_0_10V:
        this->value = map(this->reading, 3360, 16368, 10000, 50000);
        break;
      default:
        this->value = 0;
    }
    return this->value;
  }
};