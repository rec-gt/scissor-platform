#ifndef AnalogInput_H
#define AnalogInput_H

#define AI_SHIFT_BITS 4
#define AI_OVERSAMPLING_FACTOR 256  // 2 ^ (2 * 4)
#define AI_MAPPING_MODE_4_20MA 0
#define AI_MAPPING_MODE_0_10V 1
#define AI_SMOOTHING_SAMPLE_SIZE 24

class AnalogInput {
private:
  byte pin;

public:
  uint16_t reading;
  uint16_t smoothedReading;
  uint16_t readinHistory[AI_SMOOTHING_SAMPLE_SIZE];
  uint16_t value;
  uint16_t bp1;
  uint16_t bp2;
  uint16_t bp3;
  uint16_t bp4;
  uint16_t bp5;
  byte mappingMode;

  AnalogInput() {}

  AnalogInput(byte pin)
    : pin(pin) {
    pinMode(pin, INPUT);
  }

  void listen() {
    uint32_t sum = 0;

    for (int i = 0; i < AI_OVERSAMPLING_FACTOR; i++) {
      sum += analogRead(this->pin);
      delayMicroseconds(1);
    }

    /*=== Update Reading ===*/
    this->reading = sum >> AI_SHIFT_BITS;

    /*=== Update Reading History ===*/
    for (size_t i = 1; i < AI_SMOOTHING_SAMPLE_SIZE; i++) {
      this->readinHistory[i - 1] = this->readinHistory[i];
    }
    this->readinHistory[AI_SMOOTHING_SAMPLE_SIZE - 1] = this->reading;

    /*=== get smoothed reading ===*/
    uint32_t smoothSum = 0;
    for (size_t i = 0; i < AI_SMOOTHING_SAMPLE_SIZE; i++) {
      smoothSum += this->readinHistory[i];
    }
    this->smoothedReading = smoothSum / AI_SMOOTHING_SAMPLE_SIZE;
  }

  uint16_t getValue(bool w = false) {  // turn smoothings on or off
    uint32_t tmpInt = 0;

    if (this->smoothedReading <= 790) {
      tmpInt = map(this->smoothedReading, 0, 790, 0, 500);
    } else if (this->smoothedReading <= 1610) {
      tmpInt = map(this->smoothedReading, 790 + 1, 1610, 501, 1000);
    } else if (this->smoothedReading <= 4061) {
      tmpInt = map(this->smoothedReading, 1610 + 1, 4061, 1001, 2500);
    } else if (this->smoothedReading <= 8151) {
      tmpInt = map(this->smoothedReading, 4061 + 1, 8151, 2501, 5000);
    } else {
      tmpInt = map(this->smoothedReading, 8151 + 1, 16306, 5001, 10000);
    }

    this->value = map(tmpInt, 0, 10000, 0, 4095);
    
    return this->value;
  }
};

#endif