#ifndef MEMORY_H
#define MEMORY_H
#include <EEPROM.h>
#include "./SubGlobals.h"

class Memory {
public:
  void writeStr(size_t addr, size_t len, const String& str) {
    size_t strLen = str.length();
    for (size_t i = 0; i < len; i++) {
      EEPROM.put(addr + i, i > strLen - 1 ? '\0' : str[i]);
    }
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

  void readStrRange(size_t addr_s, size_t addr_e, String& target) {
    target = F("");
    for (size_t i = addr_s; i < addr_e; i++) {
      char c = EEPROM.read(i);
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
