#ifndef AnalogInput_H
#define AnalogInput_H

#define AI_SHIFT_BITS 4
#define AI_OVERSAMPLING_FACTOR 256  // 2 ^ (2 * 4)
#define AI_MAPPING_MODE_4_20MA 0
#define AI_MAPPING_MODE_0_10V 1
#define AI_SMOOTHING_SAMPLE_SIZE 16

class AnalogInput {
private:
  byte pin;
  byte mappingMode;

public:
  uint16_t reading;
  uint16_t maxReading;
  uint16_t smoothedReading;
  uint16_t readings[AI_SMOOTHING_SAMPLE_SIZE];
  unsigned long value;

  AnalogInput() {}

  AnalogInput(byte pin, byte mappingMode = AI_MAPPING_MODE_4_20MA)
    : pin(pin), mappingMode(mappingMode) {
    pinMode(pin, INPUT);
  }

  void listen() {
    uint32_t sum = 0;

    for (int i = 0; i < AI_OVERSAMPLING_FACTOR; i++) {
      sum += analogRead(this->pin);
      delayMicroseconds(1);
    }

    /*=== Update Reading ===*/
    // this->reading = (sum / AI_OVERSAMPLING_FACTOR) << AI_SHIFT_BITS;
    this->reading = sum >> AI_SHIFT_BITS;

    /*=== Update Reading History ===*/
    for (size_t i = 1; i < AI_SMOOTHING_SAMPLE_SIZE; i++) {
      this->readings[i - 1] = this->readings[i];
    }
    this->readings[AI_SMOOTHING_SAMPLE_SIZE - 1] = this->reading;

    /*=== get smoothed reading ===*/
    uint32_t smoothSum = 0;
    for (size_t i = 0; i < AI_SMOOTHING_SAMPLE_SIZE; i++) {
      smoothSum += this->readings[i];
    }
    this->smoothedReading = smoothSum / AI_SMOOTHING_SAMPLE_SIZE;
  }


  uint16_t getValue(bool w = false) {  // turn smoothings on or off
    switch (this->mappingMode) {
      case AI_MAPPING_MODE_4_20MA:
        if (this->smoothedReading <= 1593) {
          this->value = map(this->smoothedReading, 0, 1593, 0, 2000);
        } else {
          this->value = map(this->smoothedReading, 1594, 8067, 2001, 4096);
        }
        break;
      case AI_MAPPING_MODE_0_10V:
        Serial.print(this->reading);
        Serial.print(", ");
        // Serial.println(analogRead(this->pin));
        this->value = map(w ? this->smoothedReading : this->reading, 0, 16384, 0, 4096);
        Serial.println(this->value);

        if (this->reading <= 784) {
          this->value = map(w ? this->smoothedReading : this->reading, 0, 784, 0, 500);
        } else if (this->reading <= 1600) {
          this->value = map(w ? this->smoothedReading : this->reading, 785, 1600, 501, 1000);
        } else if (this->reading <= 4048) {
          this->value = map(w ? this->smoothedReading : this->reading, 1601, 4048, 1001, 2500);
        } else if (this->reading <= 8144) {
          this->value = map(w ? this->smoothedReading : this->reading, 4049, 8144, 2501, 5000);
        } else {
          this->value = map(w ? this->smoothedReading : this->reading, 8145, 16304, 5001, 10000);
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