#ifndef AnalogInput_H
#define AnalogInput_H

#define AI_SHIFT_BITS 4
#define AI_OVERSAMPLING_FACTOR 256  // 2 ^ (2 * 4)
#define AI_MAPPING_MODE_4_20MA 0
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
  uint16_t value;

  AnalogInput() {}

  AnalogInput(byte pin, byte mappingMode = AI_MAPPING_MODE_4_20MA)
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

    /*=== get weighted reading ===*/
    for (size_t i = 1; i < AI_EWMA_SAMPLE_SIZE; i++) {
      this->ewma[i] = (AI_EWMA_ALPHA * this->readings[i]) + (1 - AI_EWMA_ALPHA) * this->ewma[i - 1];
    }
    this->weightedReading = this->ewma[AI_EWMA_SAMPLE_SIZE - 1];
  }


  uint16_t getValue(bool w = false) {  // turn ewma on or off
    switch (this->mappingMode) {
      case AI_MAPPING_MODE_4_20MA:
        if (this->reading < 48) {
          this->value = 0;
        } else if (this->reading < 1584) {
          this->value = map(w ? this->weightedReading : this->reading, 48, 1583, 200, 3999);
        } else {
          this->value = map(w ? this->weightedReading : this->reading, 1584, 8048, 4000, 20000);
        }
        break;
      case AI_MAPPING_MODE_0_10V:
        if (this->reading < 128) {
          this->value = map(w ? this->weightedReading : this->reading, 0, 127, 0, 99);
        } else if (this->reading < 128) {
          this->value = map(w ? this->weightedReading : this->reading, 0, 127, 0, 99);
        } else if (this->reading < 784) {
          this->value = map(w ? this->weightedReading : this->reading, 0, 783, 0, 499);
        } else if (this->reading < 1600) {
          this->value = map(w ? this->weightedReading : this->reading, 0, 1599, 0, 1000);
        } else if (this->reading < 8144) {
          this->value = map(w ? this->weightedReading : this->reading, 0, 8143, 0, 4999);
        } else {
          this->value = map(w ? this->weightedReading : this->reading, 8144, 16304, 5000, 10000);
        }
        break;
      default:
        this->value = 0;
    }
    return this->value;
  }

  void readPlain() {
    Serial.println(analogRead(this->pin));
  }
};

#endif