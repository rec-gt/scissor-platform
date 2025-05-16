#include "Utils.h"
#include "WarningSystem.h"

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
    this->CSQ = this->response.substring(0, 8 + 2);
  }

  void parseIMEI() {
    this->IMEI = this->response.substring(this->RN + 6, this->RN + 6 + 15);
  }

  void errHook(bool add) {
    delay(300);
    Serial.println("ERR HOOK");

    if (add) {
      ++this->errCount;
    }

    if (this->errCount >= 10) {
      Serial.println("MQTT init failed");
      while (1) {};
    }

    delay(1000);
  }

public:
  String CIMI;
  String CSQ;
  String IMEI;
  String resCode;
  unsigned long prevMillis;

  NBIoT(){};

  bool sendCMD(String cmd, int time = 300) {
    this->clearBuffer();
    Serial.println("CMD: " + cmd);
    NBIoT_Serial.println(cmd);
    delay(time);  // according to docs, wait at least 300ms

    if (NBIoT_Serial.available()) {
      this->response = this->readRes();
    }
  }

  bool sendCMD2(String cmd) {
    this->clearBuffer();
    Serial.println("CMD: " + cmd);
    NBIoT_Serial.println(cmd);
    delay(500);  // according to docs, wait at least 300ms

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

  String readRes() {
    String res = "";
    while (NBIoT_Serial.available()) {
      char c = NBIoT_Serial.read();
      res += c;
    }
    return res;
  }

  int findIdx(String content, String target) {
    int startIdx = content.indexOf(target);
    Serial.print(startIdx);
    Serial.print(" : ");
    Serial.println();
    if (startIdx > -1) {
      startIdx += target.length();
    }
    return startIdx;
  }

  bool isOK() {
    return findIdx(this->response, "OK") > -1;
  }

  void init() {
    delay(100);

    NBIoT_Serial.begin(9600);

    while (1) {
      this->sendCMD("AT+QSCLK=0");
      if (this->isOK()) {
        break;
      }
      this->errHook(true);
    }

    while (1) {
      this->sendCMD("AT");
      if (this->isOK()) {
        break;
      }
      this->errHook(true);
    }

    while (1) {
      this->sendCMD("AT+CSQ");

      int startIdx = findIdx(this->response, "+CSQ: ");
      if (startIdx > -1) {
        String csq = this->response.substring(startIdx, startIdx + 2);
        if (utils.isNumeric(csq) && csq != "99") {
          this->CSQ = csq;
          break;
        }
      }

      this->errHook(true);
    }

    while (1) {
      this->sendCMD("AT+CEREG?");
      Serial.print(this->response);

      if (this->isOK()) {
        break;
      }

      this->errHook(true);
    }

    while (1) {
      this->sendCMD("AT+CGATT?");
      Serial.print(this->response);

      if (this->isOK()) {
        break;
      }

      this->errHook(true);
    }

    while (1) {
      this->sendCMD("AT+CIMI");
      String cimi = this->response.substring(10, 10 + 15);
      Serial.println("CIMI: " + cimi);
      this->CIMI = cimi;
      break;
    }

    while (1) {
      this->sendCMD("AT+CGSN=1");

      if (this->isOK()) {
        int startIdx = findIdx(this->response, "+CGSN: ");
        if (startIdx > -1) {
          this->IMEI = this->response.substring(startIdx, startIdx + 15);
          break;
        }
      }

      this->errHook(true);
    }

    Serial.println("MQTT Init Finished");

    this->connect();
  }


  void connect() {
    Serial.println("Reconnecting...");
    while (1) {
      Serial.println("Try reconnect...");

      this->sendCMD("AT+QMTCLOSE=0", 1000);

      this->sendCMD("AT+QMTDISC=1", 1000);

      this->sendCMD("AT+QMTOPEN=0,8.210.84.24,1880", 1000);

      this->sendCMD("AT+QMTCONN=0,dev,tswh,1Wo=[6vA0m", 1000);

      this->sendCMD("AT+QMTSUB=0,1,\"rgt/" + this->IMEI + "/in\",2", 1000);

      if (this->isOK()) {
        break;
      }
    }
  }

  void reconnect() {
    if (millis() - this->prevMillis > 60 * 1000) {
      this->connect();
      this->prevMillis = millis();
    }
  }

  void listen() {
    Serial.println("listen...");

    // handle reconnection
    this->reconnect();

    // handle receive data
    if (NBIoT_Serial.available()) {
      this->response = this->readRes();
      int isActived = this->response.indexOf("{\"din\":1}");
      Serial.println(this->response + String(isActived));

      warningSystem.setIsActived(isActived > -1);

      this->clearBuffer();
    }
  }

  ~NBIoT(){};
};

extern NBIoT nbiot;

#endif