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

  byte valueHistorySize = 7;
  uint16_t valueHistory[7] = {};
  byte idx = 0;
  uint16_t majorValue = 0;

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
    this->value = (avg / 32.);
    this->updateValueHistory();
    this->majorValue = this->findMostFrequentValue(this->valueHistory, this->valueHistorySize);
  }

  void updateValueHistory() {
    for (size_t i = 1; i < this->valueHistorySize; i++) {
      this->valueHistory[i - 1] = this->valueHistory[i];
    }
    this->valueHistory[this->valueHistorySize - 1] = this->value;
  }

  uint16_t findMostFrequentValue(uint16_t arr[], int size) {
    int mostFrequent = 0;
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

  uint16_t getValue() {
    return this->value;
  }
};

#endif