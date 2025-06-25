#ifndef Utils_h
#define Utils_h

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

  bool strContain(String str, char* target) {
    return str.indexOf(target) != -1;
  }

  String retrieveMsg(String mqttMsg) {
    int startInx = mqttMsg.indexOf('{');
    return (startInx != -1) ? mqttMsg.substring(startInx) : "";
  }

  int findStrIdx(String content, String target) {
    int startIdx = content.indexOf(target);
    Serial.println("FOUND AT IDX: " + String(startIdx));
    if (startIdx > -1) {
      startIdx += target.length();
    }
    return startIdx;
  }

  ~Utils(){};
};

extern Utils utils;

#endif
