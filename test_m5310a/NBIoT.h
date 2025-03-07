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
    byte RN = 2;
    this->IMEI = this->response.substring(RN + 6, RN + 6 + 15);
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
      this->sendCMD("AT+MQTTCFG=\"iot.rec-gt.com\",1880,\"" + this->IMEI + "\",60,\"tswh\",\"1Wo=[6vA0m\",1");
      if (!this->resContain("ERROR")) {
        break;
      } else {
        errHook(true);
        delay(3000);
      }
    }

    while (1) {
      this->sendCMD("AT+MQTTOPEN=1,1,1,0,1,\"rgt/" + this->IMEI + "/dev\",\"gone\"");
      if (!this->resContain("ERROR")) {
        break;
      } else {
        errHook(true);
        delay(3000);
      }
    }

    Serial.println("MQTT Init Finished");
  }

  bool sendCMD(String cmd, uint32_t timeout = 3000) {
    delay(1000);

    unsigned long deadline = millis() + timeout;  // max = 24*60*60*1000 (86400000 / 1day), default 1s
    NBIoTModule.println(cmd);

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