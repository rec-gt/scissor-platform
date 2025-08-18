#include "Globals.h"

#ifndef AnalogInput_H
#define AnalogInput_H

class AnalogInput {
private:
  byte pin;
  byte mappingMode;
  uint16_t reading;
  uint16_t weightedReading;
  uint16_t ewma[AI_EWMA_SAMPLE_SIZE];
  uint16_t readings[AI_EWMA_SAMPLE_SIZE];
  uint16_t value;
  uint16_t weightedValue;

public:
  AnalogInput() {}
  
  AnalogInput(byte pin, byte mappingMode = AI_MAPPING_MODE_4_20MA)
    : pin(pin), mappingMode(mappingMode) {
    pinMode(pin, INPUT);
  }

  void listen() {
    uint32_t sum = 0;

    for (int i = 0; i < OVERSAMPLING_FACTOR; i++) {
      sum += analogRead(this->pin);
    }

    /*=== Update Reading ===*/
    this->reading = (sum / OVERSAMPLING_FACTOR) << SHIFT_BITS;

    /*=== Update Reading History ===*/
    for (size_t i = 1; i < AI_EWMA_SAMPLE_SIZE; i++) {
      this->readings[i - 1] = this->readings[i];
    }
    this->readings[AI_EWMA_SAMPLE_SIZE - 1] = this->reading;
  }

  unsigned int getReading(bool w = true) {
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

  unsigned int getValue(bool w = true) {  // turn ewma on or off
    switch (this->mappingMode) {
      case AI_MAPPING_MODE_4_20MA:
        this->value = map(w ? this->weightedReading : this->reading, 0, 16368, 4, 20000);
        break;
      case AI_MAPPING_MODE_0_10V:
        this->value = map(w ? this->weightedReading : this->reading, 3360, 16368, 10000, 50000);
        break;
      default:
        this->value = 0;
    }
    return this->value;
  }

  void debug() {
    for (size_t i = 0; i < AI_EWMA_SAMPLE_SIZE; i++) {
      Serial.print(this->readings[i]);
      Serial.print(", ");
    }
    Serial.print(" || ");
    for (size_t i = 0; i < AI_EWMA_SAMPLE_SIZE; i++) {
      Serial.print(this->ewma[i]);
      Serial.print(", ");
    }
  }
};

#endif