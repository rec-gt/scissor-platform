#ifndef UTILS_H
#define UTILS_H

class Utils {
public:
  Utils(void){};

  char* concatCharN(char** charArr, size_t arrSize) {
    int totalCharLen = 0;
    for (size_t i = 0; i < arrSize; i++) {
      totalCharLen += strlen(charArr[i]);
    }
    char* newChar = new char[totalCharLen + 1];

    newChar[0] = '\0';

    for (size_t i = 0; i < arrSize; i++) {
      strcat(newChar, charArr[i]);
    }

    return newChar;
  }

  char* num2Char(int num) {
    char cstr[16];
    char* c = itoa(num, cstr, 10);
    return c;
  }

  bool isNumeric(String str) {
    for (size_t i = 0; i < str.length(); i++) {
      if (!isDigit(str.charAt(i))) {
        return false;
      }
    }

    return true;
  }

  ~Utils(){};
};

extern Utils utils;

#endif
