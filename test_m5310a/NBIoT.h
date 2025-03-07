#define NBIoTModule Serial1

class NBIoT {
private:
  byte errCount = 0;
  String IMEI;
  String response;

  void clearBuffer() {
    while (NBIoTModule.read() >= 0) {}
  }

  bool resContain(char* target) {
    return this->response.indexOf(target) != -1;
  }

  void parseIMEI() {
    this->IMEI = this->response.substring(6, 15);
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

    this->sendCMD("AT+CLAC");

    // ask for 9600 baud rate
    while (1) {
      this->sendCMD("AT+NATSPEED=9600,30,0,0");
      if (!this->resContain("ERROR")) {
        break;
      } else {
        errHook(true);
        delay(3000);
      }
    }

    // check communication success
    while (1) {
      this->sendCMD("AT");
      if (!this->resContain("ERROR")) {
        break;
      } else {
        errHook(true);
        delay(3000);
      }
    }

    // get cimi
    while (1) {
      this->sendCMD("AT+CIMI");
      if (!this->resContain("ERROR")) {
        break;
      } else {
        errHook(true);
        delay(3000);
      }
    }

    while (1) {
      this->sendCMD("AT+CSQ");
      if (!this->resContain("ERROR")) {
        break;
      } else {
        errHook(true);
        delay(3000);
      }
    }

    while (1) {
      this->sendCMD("AT+CEREG?");
      if (!this->resContain("ERROR")) {
        break;
      } else {
        errHook(true);
        delay(3000);
      }
    }

    while (1) {
      this->sendCMD("AT+CEREG=1");
      if (!this->resContain("ERROR")) {
        break;
      } else {
        errHook(true);
        delay(3000);
      }
    }

    while (1) {
      this->sendCMD("AT+CGATT?");
      if (!this->resContain("ERROR")) {
        break;
      } else {
        errHook(true);
        delay(3000);
      }
    }

    while (1) {
      this->sendCMD("AT+CGSN=1");
      if (!this->resContain("ERROR")) {
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
      if (!this->resContain("ERROR")) {
        break;
      } else {
        errHook(true);
        delay(3000);
      }
    }

    while (1) {
      this->sendCMD("AT+MQTTOPEN=1,1,1,0,1,\"rgt/869976034806621/dev\",\"gone\"");
      if (!this->resContain("ERROR")) {
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

  bool sendCMD(String cmd, uint32_t timeout = 5000, uint32_t delayMS = 0) {
    delay(delayMS);
    unsigned long deadline = millis() + timeout;  // max = 24*60*60*1000 (86400000 / 1day), default 1s
    NBIoTModule.println(cmd);

    delay(3000);

    while (millis() < deadline) {
      if (NBIoTModule.available()) {
        this->response = NBIoTModule.readString();
        Serial.println(cmd + ": ");
        Serial.println(this->response);
        this->clearBuffer();
        return true;
      }
    }
    this->clearBuffer();
    return false;
  }

  ~NBIoT(){};
};