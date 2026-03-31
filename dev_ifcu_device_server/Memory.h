#ifndef MEMORY_H
#define MEMORY_H
#include "./Globals.h"

class Memory {
public:
  void writeStr(size_t addr, size_t len, const String& str) {
    size_t strLen = str.length();
    for (size_t i = 0; i < len; i++) {
      EEPROM.write(addr + i, i > strLen - 1 ? '\0' : str[i]);
    }
    EEPROM.commit();
  }

  void readStr(size_t addr, size_t len, String& target) {
    target = F("");
    for (size_t i = 0; i < len; i++) {
      char c = EEPROM.read(addr + i);
      if (c != '\0') {
        target += c;
      } else {
        break;
      }
    }
  }
};

extern Memory memory;

#endif
