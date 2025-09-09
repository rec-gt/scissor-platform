#include "Globals.h"

#ifndef AnalogInput_H
#define AnalogInput_H

#define AI_SHIFT_BITS 4
#define AI_OVERSAMPLING_FACTOR 256  // 2 ^ (2 * 4)
#define AI_MAPPING_MODE_0_20MA 0
#define AI_MAPPING_MODE_0_10V 1
#define AI_EWMA_SAMPLE_SIZE 4
#define AI_EWMA_ALPHA 0.2

class AnalogInput {
private:
  byte pin;
  byte mappingMode;

public:
  uint16_t reading;
  uint16_t weightedReading;
  uint16_t ewma[AI_EWMA_SAMPLE_SIZE];
  uint16_t readings[AI_EWMA_SAMPLE_SIZE];
  uint16_t weightedValue;
  uint16_t value;

  AnalogInput() {}

  AnalogInput(byte pin, byte mappingMode = AI_MAPPING_MODE_0_20MA)
    : pin(pin), mappingMode(mappingMode) {
    pinMode(pin, INPUT);
  }

  void listen() {
    uint32_t sum = 0;

    for (int i = 0; i < AI_OVERSAMPLING_FACTOR; i++) {
      sum += analogRead(this->pin);
    }

    /*=== Update Reading ===*/
    this->reading = (sum / AI_OVERSAMPLING_FACTOR) << AI_SHIFT_BITS;

    /*=== Update Reading History ===*/
    for (size_t i = 1; i < AI_EWMA_SAMPLE_SIZE; i++) {
      this->readings[i - 1] = this->readings[i];
    }
    this->readings[AI_EWMA_SAMPLE_SIZE - 1] = this->reading;
  }

  uint16_t getReading(bool w = true) {
    if (w) {
      for (size_t i = 1; i < AI_EWMA_SAMPLE_SIZE; i++) {
        this->ewma[i] = (AI_EWMA_ALPHA * this->readings[i]) + (1 - AI_EWMA_ALPHA) * this->ewma[i - 1];
      }
      this->weightedReading = this->ewma[AI_EWMA_SAMPLE_SIZE - 1];
      return this->weightedReading;
    } else {
      return this->reading;
    }
  }

  uint16_t getValue(bool w = true) {  // turn ewma on or off
    switch (this->mappingMode) {
      case AI_MAPPING_MODE_0_20MA:
        this->value = map(w ? this->weightedReading : this->reading, 0, 3919, 0, 20000);  // fine-tuned
        break;
      case AI_MAPPING_MODE_0_10V:
        this->value = map(w ? this->weightedReading : this->reading, 0, 7885, 0, 10000);  // fine-tuned
        break;
      default:
        this->value = 0;
    }
    return this->value;
  }
};

#endif