
#ifndef NBIoT_h
#define NBIoT_h
#define NBIoTModule Serial1

class NBIoT {
private:
  byte errCount = 0;
  byte RN = 2;
  String CIMI;
  String CSQ;
  String IMEI;
  String response;

  unsigned long previousMillis = 0;

  void clearBuffer() {
    while (NBIoTModule.read() >= 0) {}
  }

  bool resContain(char* target) {
    return this->response.indexOf(target) != -1;
  }

  void parseCIMI() {
    this->CIMI = this->response.substring(this->RN + 0, this->RN + 15);
    Serial.println(this->CIMI);
  }

  void parseCSQ() {
    this->CSQ = this->response.substring(this->RN + 5, this->RN + 5 + 2);
    Serial.println(this->CSQ);
  }

  void parseIMEI() {
    this->IMEI = this->response.substring(this->RN + 6, this->RN + 6 + 15);
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

  bool sendCMD(String cmd, uint32_t timeout = 1000) {
    delay(100);
    unsigned long deadline = millis() + timeout;  // max = 24*60*60*1000 (86400000 / 1day), default 1s
    NBIoTModule.println(cmd);
    delay(100);

    while (millis() < deadline) {
      if (NBIoTModule.available()) {
        this->response = NBIoTModule.readString();
        Serial.println("CMD: " + cmd);
        Serial.println(this->response);
        this->clearBuffer();
        return true;
      }
    }
    this->clearBuffer();
    return false;
  }

  void sendCMDFast(String cmd) {
    Serial.println("CMD: " + cmd);
    NBIoTModule.println(cmd);
    this->clearBuffer();
  }

  bool init() {
    NBIoTModule.begin(9600);

    this->clearBuffer();

    // ask for 9600 baud rate
    while (1) {
      this->sendCMD("AT+NATSPEED=9600,30,0,0");
      if (!this->resContain("ERROR")) {
        break;
      } else {
        this->errHook(true);
        delay(1000);
      }
    }

    // check communication success
    while (1) {
      this->sendCMD("AT");
      if (!this->resContain("ERROR")) {
        break;
      } else {
        this->errHook(true);
        delay(1000);
      }
    }

    while (1) {
      this->sendCMD("AT+CSQ");
      if (!this->resContain("ERROR")) {
        break;
      } else {
        this->errHook(true);
        delay(1000);
      }
    }
    this->parseCSQ();

    while (1) {
      this->sendCMD("AT+CEREG?");
      if (!this->resContain("ERROR")) {
        break;
      } else {
        this->errHook(true);
        delay(1000);
      }
    }

    while (1) {
      this->sendCMD("AT+CGATT?");
      if (!this->resContain("ERROR")) {
        break;
      } else {
        this->errHook(true);
        delay(1000);
      }
    }

    // get cimi
    while (1) {
      this->sendCMD("AT+CIMI");
      if (!this->resContain("ERROR")) {
        break;
      } else {
        this->errHook(true);
        delay(1000);
      }
    }
    this->parseCIMI();



    while (1) {
      this->sendCMD("AT+CGSN=1");
      if (!this->resContain("ERROR")) {
        break;
      } else {
        this->errHook(true);
        delay(1000);
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
        this->errHook(true);
        delay(1000);
      }
    }

    while (1) {
      this->sendCMD("AT+MQTTOPEN=1,1,1,0,1,\"rgt/" + this->IMEI + "/dev\",\"gone\"");
      if (!this->resContain("ERROR")) {
        break;
      } else {
        this->errHook(true);
        delay(1000);
      }
    }

    Serial.println("MQTT Init Finished");

    if (this->errCount > 10) {
      return false;
    }
    return true;
  }

  void publish(byte sensors8Status, byte sensors2Status, SystemStatus systemStatus, bool isLiftedUp) {
    // Digital Input + Output = sensorsStatus
    // Analog Input[0] : 1 = RUNNING, ...
    // Analog Input[1] : 0 = not lifted up, 1 = lifted up

    if (millis() - previousMillis >= 10 * 1000) {
      previousMillis = millis();
      this->sendCMDFast("AT+MQTTPUB=\"rgt/" + String(this->IMEI) + "/in\",1,0,0,0,\"{\"seq\":1,\"csq\":" + String(this->CSQ) + ",\"sw\":0,\"din\":" + String(sensors8Status) + ",\"dout\":" + String(sensors2Status) + ",\"ain\":[" + String(systemStatus) + "," + String(isLiftedUp) + ",0,0],\"aout\":[0,0,0,0]}\"");
    }
  }

  ~NBIoT(){};
};

extern NBIoT nbiot;

#endif