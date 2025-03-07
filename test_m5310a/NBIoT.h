#define NBIoTModule Serial1

class NBIoT {
private:
  byte errCount = 0;
  String response = "";  // 節省空間
  char* IMEI = "";

  // response management
  char* res = NULL;
  int resIdx = 0;


  void clearBuffer() {
    while (NBIoTModule.read() >= 0) {}
  }

  char handleGetRes() {
    free(this->res);
    this->res = (char*)malloc(NBIoTModule.available() + 1);
    this->resIdx = 0;

    while (NBIoTModule.available() > 0) {
      char c = NBIoTModule.read();
      this->res[this->resIdx++] = c;
    }
    this->res[this->resIdx] = '\0';

    Serial.println(this->res);
  }

  bool resContain(const char* target) {
    for (int i = 0; this->res[i] != '\0'; i++) {
      bool found = true;
      for (int j = 0; target[j] != '\0'; j++) {
        if (this->res[i + j] != target[j]) {
          found = false;
          break;
        }
      }
      if (found) {
        return true;
      }
    }
    return false;
  }

  char* selectChar(char* str, byte start, byte length) {
    length += 2;  // idk why
    char* newStr = new char[length + 1];
    strncpy(newStr, str + start, length);
    newStr[length] = '\0';
    return newStr;
  }

  void parseCIMI() {
    Serial.println(this->selectChar(this->res, 0, 15));
  }

  void parseIMEI() {
    this->IMEI = this->selectChar(this->res, 8, 15);
    Serial.println(this->IMEI);
  }

  void errHook(bool add) {
    if (add) {
      ++this->errCount;
    }

    if (this->errCount >= 10) {
      Serial.println("MQTT init failed");
    }
  }

public:
  NBIoT(){};

  void init() {
    Serial.println("Initiating MQTT Module");

    NBIoTModule.begin(9600);
    this->clearBuffer();

    // ask for 9600 baud rate
    while (1) {
      this->sendCMD("AT+NATSPEED=9600,30,0,0");
      if (this->resContain("OK")) {
        break;
      } else {
        errHook(true);
        delay(3000);
      }
    }

    // check communication success
    while (1) {
      this->sendCMD("AT");
      if (this->resContain("OK")) {
        break;
      } else {
        errHook(true);
        delay(3000);
      }
    }

    // get cimi
    while (1) {
      this->sendCMD("AT+CIMI");
      if (this->resContain("OK")) {
        break;
      } else {
        errHook(true);
        delay(3000);
      }
    }

    while (1) {
      this->sendCMD("AT+CSQ");
      if (this->resContain("OK")) {
        break;
      } else {
        errHook(true);
        delay(3000);
      }
    }

    while (1) {
      this->sendCMD("AT+CEREG?");
      if (this->resContain("OK")) {
        break;
      } else {
        errHook(true);
        delay(3000);
      }
    }

    while (1) {
      this->sendCMD("AT+CEREG=1");
      if (this->resContain("OK")) {
        break;
      } else {
        errHook(true);
        delay(3000);
      }
    }

    while (1) {
      this->sendCMD("AT+CGATT?");
      if (this->resContain("OK")) {
        break;
      } else {
        errHook(true);
        delay(3000);
      }
    }

    while (1) {
      this->sendCMD("AT+CGSN=1");
      if (this->resContain("OK")) {
        break;
      } else {
        errHook(true);
        delay(3000);
      }
    }
    this->parseIMEI();


    while (1) {
      this->sendCMD("AT+MQTTDISC");
      this->sendCMD("AT+MQTTDEL");
      this->sendCMD("AT+MQTTCFG=\"iot.rec-gt.com\",1880,\"869976034806621\",60,\"tswh\",\"1Wo=[6vA0m\",1");
      if (this->resContain("OK")) {
        break;
      } else {
        errHook(true);
        delay(3000);
      }
    }

    while (1) {
      this->sendCMD("AT+MQTTOPEN=1,1,1,0,1,\"rgt/869976034806621/dev\",\"gone\"");
      if (this->resContain("OK")) {
        break;
      } else {
        errHook(true);
        delay(3000);
      }
    }

    Serial.println("MQTT Init Finished");
  }

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

  bool sendCMD(String cmd, uint32_t timeout = 1000, uint32_t delayMS = 0) {
    delay(delayMS);
    unsigned long deadline = millis() + timeout;  // max = 24*60*60*1000 (86400000 / 1day), default 1s
    NBIoTModule.println(cmd);

    while (millis() < deadline) {
      if (NBIoTModule.available()) {
        Serial.print(cmd + ": ");
        this->handleGetRes();
        this->clearBuffer();
        return true;
      }
    }
    this->clearBuffer();
    return false;
  }

  ~NBIoT(){};
};