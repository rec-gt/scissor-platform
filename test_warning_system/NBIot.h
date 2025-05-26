#include "Utils.h"
#include "WarningSystem.h"

#ifndef NBIoT_h
#define NBIoT_h
#define NBIoT_Serial Serial1

class NBIoT {
private:
  byte errCount = 0;
  String response;

  void clearBuffer() {
    while (NBIoT_Serial.read() >= 0) {}
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
  unsigned long prevMillis = 0;

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
    while (1) {
      Serial.println("Try connecting...");

      this->sendCMD("AT+QMTDISC=1", 1500);

      this->sendCMD("AT+QMTCLOSE=0", 1500);

      this->sendCMD("AT+QMTDISC=1", 1500);

      this->sendCMD("AT+QMTOPEN=0,8.210.84.24,1880", 1500);

      this->sendCMD("AT+QMTCONN=0,dev,tswh,1Wo=[6vA0m", 1500);

      this->sendCMD("AT+QMTSUB=0,1,\"rgt/" + this->IMEI + "/in\",2", 2000);

      if (this->isOK()) {
        break;
      }
    }
  }

  void checkReconnect() {
    unsigned long currMillis = millis();
    int interval = 1800000;  // 30*60*1000
    if ((currMillis - this->prevMillis) > interval) {
      Serial.println("Reconnecting...");
      this->connect();
      this->prevMillis = currMillis;
    }
  }

  void listen() {
    Serial.println("listen...");

    // handle reconnection
    this->checkReconnect();
    unsigned long currMillis = millis();
    Serial.println(String(currMillis - this->prevMillis) + " : " + String(bool((currMillis - this->prevMillis) > long(30 * 60 * 1000))));

    // handle receive data
    if (NBIoT_Serial.available()) {
      this->response = this->readRes();
      int isReceiving = this->response.indexOf("+QMTRECV:");

      if (isReceiving > -1) {
        int isActived = this->response.indexOf("{\"din\":1}");
        Serial.println(this->response + String(isActived));
        warningSystem.setIsActived(isActived > -1);
      } else {
        Serial.println(this->response);
      }

      this->clearBuffer();
    }
  }

  ~NBIoT(){};
};

extern NBIoT nbiot;

#endif