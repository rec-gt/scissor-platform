#ifndef GLOBALS_H
#define GLOBALS_H

#define SHIFT_BITS 4
#define OVERSAMPLING_FACTOR 256  // 2 ^ (2 * 4)

// 經驗數值
// Serial.println(map(analogInputs[i].getValue(), 0, 16368, 0, 50000));// for 0-5V
// Serial.println(map(analogInputs[i].getValue(), 3360, 16368, 10000, 50000)); // for 4-20mA

#endif