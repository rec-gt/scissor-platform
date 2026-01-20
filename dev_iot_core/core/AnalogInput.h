#ifndef AnalogInput_H
#define AnalogInput_H

#define AI_SHIFT_BITS 4
#define AI_OVERSAMPLING_FACTOR 256  // 2 ^ (2 * 4)
#define AI_SMOOTHING_SAMPLE_SIZE 24

class AnalogInput {
protected:
  byte pin;
  byte resolution = 0;  // 0=1024, 1=4096

public:
  uint16_t reading;
  uint16_t readingHistory[AI_SMOOTHING_SAMPLE_SIZE];
  uint16_t smoothedReading;
  uint16_t value;

  AnalogInput() {}

  AnalogInput(byte pin, byte resolution = 0)
    : pin(pin), resolution(resolution) {
    pinMode(pin, INPUT);
  }

  void setResolution(byte resolution) {
    this->resolution = resolution;
  }

  void listen() {
    /* === For 4096 Resolution === */
    if (this->resolution == 1) {
      uint32_t sum = 0;

      for (int i = 0; i < AI_OVERSAMPLING_FACTOR; i++) {
        sum += analogRead(this->pin);
        delayMicroseconds(1);
      }

      /*=== Update Reading ===*/
      this->reading = sum >> AI_SHIFT_BITS;

      /*=== Update Reading History ===*/
      for (size_t i = 1; i < AI_SMOOTHING_SAMPLE_SIZE; i++) {
        this->readingHistory[i - 1] = this->readingHistory[i];
      }
      this->readingHistory[AI_SMOOTHING_SAMPLE_SIZE - 1] = this->reading;

      /*=== get smoothed reading ===*/
      uint32_t smoothSum = 0;
      for (size_t i = 0; i < AI_SMOOTHING_SAMPLE_SIZE; i++) {
        smoothSum += this->readingHistory[i];
      }
      this->smoothedReading = smoothSum / AI_SMOOTHING_SAMPLE_SIZE;
    }

    /* === For 1024 Resolution === */
    else {
      uint32_t smoothSum = 0;
      for (size_t i = 0; i < 64; i++) {
        smoothSum += analogRead(this->pin);
      };
      this->smoothedReading = (smoothSum / 64);
    }

    /* === Parse to value === */
    /* === For 4096 Resolution === */
    if (this->resolution == 1) {
      this->value = map(constrain((int32_t)this->smoothedReading, 0, 16063), 0, 16063, 0, 4095);
    }
    /* === For 1024 Resolution === */
    else {
      this->value = constrain((int32_t)this->smoothedReading, 0, 1023);
    }
  }

  uint16_t getValue() {
    return this->value;
  }
};

#endif