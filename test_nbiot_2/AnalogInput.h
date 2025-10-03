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
  uint16_t readings[AI_SMOOTHING_SAMPLE_SIZE];
  uint16_t value;
  uint16_t bp1;
  uint16_t bp2;
  uint16_t bp3;
  uint16_t bp4;
  uint16_t bp5;
  byte mappingMode;

  AnalogInput() {}

  AnalogInput(byte pin, byte mappingMode = AI_MAPPING_MODE_4_20MA)
    : pin(pin), mappingMode(mappingMode) {
    pinMode(pin, INPUT);
  }

  AnalogInput(byte pin, byte mappingMode, int bp1, int bp2, int bp3, int bp4, int bp5)
    : pin(pin), mappingMode(mappingMode), bp1(bp1), bp2(bp2), bp3(bp3), bp4(bp4), bp5(bp5) {
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
    uint32_t tmpInt = 0;
    switch (this->mappingMode) {
      case AI_MAPPING_MODE_4_20MA:
        if (this->smoothedReading <= 1591) {
          tmpInt = map(this->smoothedReading, 0, 1591, 0, 4000);
        } else if (this->smoothedReading <= 3204) {
          tmpInt = map(this->smoothedReading, 1591 + 1, 3204, 4001, 8000);
        } else if (this->smoothedReading <= 4817) {
          tmpInt = map(this->smoothedReading, 3204 + 1, 4817, 8001, 12000);
        } else if (this->smoothedReading <= 6437) {
          tmpInt = map(this->smoothedReading, 4817 + 1, 6437, 12001, 16000);
        } else {
          tmpInt = map(this->smoothedReading, 6437 + 1, 8051, 16001, 20000);
        }
        this->value = map(tmpInt, 0, 20000, 0, 4095);
        break;
      case AI_MAPPING_MODE_0_10V:
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
        break;
      default:
        this->value = 0;
    }
    return this->value;
  }

  // uint16_t getValue(bool w = false) {  // turn smoothings on or off
  //   switch (this->mappingMode) {
  //     case AI_MAPPING_MODE_4_20MA:
  //       if (this->smoothedReading <= this->bp1) {
  //         this->value = map(this->smoothedReading, 0, this->bp1, 0, 4000);
  //       } else if (this->smoothedReading <= this->bp2) {
  //         this->value = map(this->smoothedReading, this->bp1 + 1, this->bp2, 4001, 8000);
  //       } else if (this->smoothedReading <= this->bp3) {
  //         this->value = map(this->smoothedReading, this->bp2 + 1, this->bp3, 8001, 12000);
  //       } else if (this->smoothedReading <= this->bp4) {
  //         this->value = map(this->smoothedReading, this->bp3 + 1, this->bp4, 12001, 16000);
  //       } else if (this->smoothedReading <= this->bp5) {
  //         this->value = map(this->smoothedReading, this->bp4 + 1, this->bp5, 16001, 20000);
  //       }
  //       this->value = map(this->value, 0, 20000, 0, 4095);
  //       break;
  //     case AI_MAPPING_MODE_0_10V:
  //       if (this->smoothedReading <= this->bp1) {
  //         this->value = map(this->smoothedReading, 0, this->bp1, 0, 500);
  //       } else if (this->smoothedReading <= this->bp2) {
  //         this->value = map(this->smoothedReading, this->bp1 + 1, this->bp2, 501, 1000);
  //       } else if (this->smoothedReading <= this->bp3) {
  //         this->value = map(this->smoothedReading, this->bp2 + 1, this->bp3, 1001, 2500);
  //       } else if (this->smoothedReading <= this->bp4) {
  //         this->value = map(this->smoothedReading, this->bp3 + 1, this->bp4, 2501, 5000);
  //       } else if (this->smoothedReading <= this->bp5) {
  //         this->value = map(this->smoothedReading, this->bp4 + 1, this->bp5, 5001, 10000);
  //       }
  //       this->value = map(this->value, 0, 10000, 0, 4095);
  //       break;
  //     default:
  //       this->value = 0;
  //   }
  //   return this->value;
  // }

  void readPlain() {
    Serial.println(analogRead(this->pin));
  }
};


class AnalogInputA : public AnalogInput {
public:
  AnalogInputA(byte pin, byte mappingMode, int bp1, int bp2, int bp3, int bp4, int bp5)
    : AnalogInput(pin, mappingMode, bp1, bp2, bp3, bp4, bp5) {}
};

class AnalogInputV : public AnalogInput {
public:
  AnalogInputV(byte pin, byte mappingMode, int bp1, int bp2, int bp3, int bp4, int bp5)
    : AnalogInput(pin, mappingMode, bp1, bp2, bp3, bp4, bp5) {}
};


#endif