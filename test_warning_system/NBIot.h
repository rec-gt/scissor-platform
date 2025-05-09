#include "Utils.h"

#ifndef NBIoT_h
#define NBIoT_h
#define NBIoT_Serial Serial1

class NBIoT {
private:
  byte errCount = 0;
  byte RN = 2;
  String response;

  void clearBuffer() {
    while (NBIoT_Serial.read() >= 0) {}
  }

  bool resContain(char* target) {
    return this->response.indexOf(target) != -1;
  }

  void parseCIMI() {
    this->CIMI = this->response.substring(this->RN + 0, this->RN + 15);
  }

  void parseCSQ() {
    this->CSQ = this->response.substring(this->RN + 5, this->RN + 5 + 2);
  }

  void parseIMEI() {
    this->IMEI = this->response.substring(this->RN + 6, this->RN + 6 + 15);
  }

  void errHook(bool add) {
    if (add) {
      ++this->errCount;
    }

    if (this->errCount >= 10) {
      Serial.println("MQTT init failed");
      while (1) {};
    }
  }

public:
  String CIMI;
  String CSQ;
  String IMEI;

  NBIoT(){};

  bool sendCMD(String cmd) {
    this->clearBuffer();
    Serial.println("CMD: " + cmd);
    NBIoT_Serial.println(cmd);
    delay(500);  // wait at least 300ms

    if (NBIoT_Serial.available()) {
      this->response = NBIoT_Serial.readString();
      Serial.println(this->response);
      this->clearBuffer();
      return true;
    } else {
      this->clearBuffer();
      return false;
    }
  }

  void init() {
    delay(100);

    NBIoT_Serial.begin(9600);

    this->clearBuffer();

    // ask for 9600 baud rate
    while (1) {
      this->sendCMD("AT+NATSPEED=9600,30,0,0");
      if (this->resContain("OK")) {
        break;
      } else {
        this->errHook(true);
        delay(1000);
      }
    }

    // check communication success
    while (1) {
      this->sendCMD("AT");
      if (this->resContain("OK")) {
        break;
      } else {
        this->errHook(true);
        delay(1000);
      }
    }

    while (1) {
      this->sendCMD("AT+CSQ");
      if (this->resContain("OK")) {
        this->parseCSQ();
        char* csq_c = this->CSQ.c_str();
        if (!utils.isNumeric(csq_c) || this->CSQ == "99") {
          this->errHook(true);
          continue;
        }
        break;
      } else {
        this->errHook(true);
        delay(1000);
      }
    }

    while (1) {
      this->sendCMD("AT+CEREG?");
      if (this->resContain("OK")) {
        break;
      } else {
        this->errHook(true);
        delay(1000);
      }
    }

    while (1) {
      this->sendCMD("AT+CGATT?");
      if (this->resContain("OK")) {
        break;
      } else {
        this->errHook(true);
        delay(1000);
      }
    }

    // get cimi
    while (1) {
      this->sendCMD("AT+CIMI");
      if (this->resContain("OK")) {
        break;
      } else {
        this->errHook(true);
        delay(1000);
      }
    }
    this->parseCIMI();

    while (1) {
      this->sendCMD("AT+CGSN=1");
      if (this->resContain("OK")) {
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
      if (this->resContain("OK")) {
        break;
      } else {
        this->errHook(true);
        delay(1000);
      }
    }

    while (1) {
      this->sendCMD("AT+MQTTOPEN=1,1,1,0,1,\"rgt/" + this->IMEI + "/in\",\"gone\"");
      if (this->resContain("OK")) {
        break;
      } else {
        this->errHook(true);
        delay(1000);
      }
    }

    while (1) {
      this->sendCMD("AT+MQTTSUB=TOPIC 123,0,0");
      delay(3000);
      if (this->resContain("OK")) {
        break;
      } else {
        this->errHook(true);
        delay(1000);
      }
    }

    Serial.println("MQTT Init Finished");
  }

  void listen() {
    if (NBIoT_Serial.available()) {
      String mqttResponse = NBIoT_Serial.readString();
      Serial.println(mqttResponse);

      utils.retrieveMsg(mqttResponse);

      this->clearBuffer();
    }
  }

  ~NBIoT(){};
};

extern NBIoT nbiot;

#endif