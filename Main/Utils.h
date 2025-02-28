#ifndef Utils_h
#define Utils_h

class Utils {
public:
  Utils(void){};

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
