#ifndef Utils_h
#define Utils_h

class Utils {
public:
  Utils(void){};

  char* num2Char(int num) {
    char cstr[16];
    char* newChar = itoa(num, cstr, 10);
    return newChar;
  }

  ~Utils(){};
};

extern Utils utils;

#endif
