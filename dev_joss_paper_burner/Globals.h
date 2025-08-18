#ifndef GLOBALS_H
#define GLOBALS_H

#define SHIFT_BITS 4
#define OVERSAMPLING_FACTOR 256  // 2 ^ (2 * 4)

#define AI_MAPPING_MODE_4_20MA 0
#define AI_MAPPING_MODE_0_10V 1
#define AI_EWMA_SAMPLE_SIZE 4
#define AI_EWMA_ALPHA 0.2
// 經驗數值
// Serial.println(map(analogInputs[i].getValue(), 0, 16368, 0, 50000));// for 0-5V
// Serial.println(map(analogInputs[i].getValue(), 3360, 16368, 10000, 50000)); // for 4-20mA

#endif