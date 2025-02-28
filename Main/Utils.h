#ifndef Utils_h
#define Utils_h

class Utils {
public:
  Utils(void){};

  char* concatCharN(char* charArr[], size_t arrSize) {
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

  char* concatChar(char* a, char* b) {
    char* newChar = new char[strlen(a) + strlen(b) + 1];
    strcpy(newChar, a);
    strcat(newChar, b);
    return newChar;
  }

  char* num2Char(int num) {
    char cstr[16];
    char* newChar = itoa(num, cstr, 10);
    return newChar;
  }

  ~Utils(){};
};

extern Utils utils;

#endif
