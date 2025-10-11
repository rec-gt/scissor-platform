#ifndef AnalogInput_H
#define AnalogInput_H

#define AI_SHIFT_BITS 4
#define AI_OVERSAMPLING_FACTOR 256  // 2 ^ (2 * 4)
#define AI_SMOOTHING_SAMPLE_SIZE 24

class AnalogInput {
protected:
  byte pin;

public:
  uint16_t reading;
  uint16_t smoothedReading;
  uint16_t readingHistory[AI_SMOOTHING_SAMPLE_SIZE];
  uint16_t value;

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

  uint16_t getValue() {
    this->value = map(this->smoothedReading, 0, 16063, 0, 4095);
    return this->value;
  }
};

class AnalogInputFaster : public AnalogInput {
private:
  uint16_t findMostFrequentValue(uint16_t arr[], int size) {
    uint16_t mostFrequent = 0;
    int maxCount = 0;

    for (int i = 0; i < size; i++) {
      int count = 0;

      for (int j = 0; j < size; j++) {
        if (arr[i] == arr[j]) {
          count++;
        }
      }

      if (count > maxCount) {
        maxCount = count;
        mostFrequent = arr[i];
      }
    }

    return mostFrequent;
  }

public:
  AnalogInputFaster(byte pin)
    : AnalogInput(pin) {}

  void listen() {
    uint32_t avg = 0;
    for (size_t i = 0; i < 32; i++) {
      avg += analogRead(this->pin);
    };
    this->reading = avg / 32;

    /*=== Update Reading History ===*/
    for (size_t i = 1; i < AI_SMOOTHING_SAMPLE_SIZE; i++) {
      this->readingHistory[i - 1] = this->readingHistory[i];
    }
    this->readingHistory[AI_SMOOTHING_SAMPLE_SIZE - 1] = this->reading;

    this->value = this->findMostFrequentValue(this->readingHistory, AI_SMOOTHING_SAMPLE_SIZE);
  }

  uint16_t getValue() {
    return this->value;
  }
};

#endif