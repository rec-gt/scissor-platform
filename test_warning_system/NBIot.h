#include "Utils.h"
#include "WarningSystem.h"
#include <avr/wdt.h>

#ifndef NBIoT_h
#define NBIoT_h
#define NBIoT_Serial Serial1

void reboot() {
  wdt_disable();
  wdt_enable(WDTO_15MS);
  while (1) {}
}

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
    Serial.println("CMD: " + cmd);
    NBIoT_Serial.println(cmd);
    delay(time);  // according to docs, wait at least 300ms

    if (NBIoT_Serial.available()) {
      this->response = this->readRes();
      Serial.println("RESPONSE: " + this->response);
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

    // init
    // this->sendCMD("AT+CFUN=0", 100);
    // this->sendCMD("AT+QCSEARFCN", 3000);
    // this->sendCMD("AT+QRST=1", 3…000);
    // this->sendCMD("AT+CFUN=1", 100);
    this->sendCMD("AT+QSCLK=0", 100);
    // this->sendCMD("AT+CPSMS=0", 100);
    // this->sendCMD("AT+CEDRXS=0,5", 100);
    // this->sendCMD("AT+CSCON=0", 100);
    // this->sendCMD("AT+QMTCFG=version,0,1", 100);
    // this->sendCMD("AT+QMTCFG=keepalive,0,0", 100);
    // this->sendCMD("AT+QMTCFG=session,0,1", 100);
    // this->sendCMD("AT+QIDNSCFG=0,8.8.8.8,1.1.1.1", 100);

    // connection
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
      delay(2000);
    }

    while (1) {
      this->sendCMD("AT+CEREG?");
      Serial.print(this->response);
      int errIdx1 = findIdx(this->response, "+CEREG:0,0");
      int errIdx2 = findIdx(this->response, "+CEREG:0,2");

      if (errIdx1 > -1 || errIdx2 > -1) {
        reboot();
      }

      if (this->isOK()) {
        break;
      }

      this->errHook(true);
      delay(2000);
    }

    while (1) {
      this->sendCMD("AT+CGATT?");
      if (this->isOK()) {
        break;
      }
      this->errHook(true);
      delay(2000);
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
    this->clearBuffer();
    delay(2000);

    this->connect();
  }


  void connect() {
    while (1) {
      Serial.println("Try connecting...");

      this->sendCMD("AT+QMTDISC=0", 1500);

      this->sendCMD("AT+QMTCLOSE=0", 1500);

      this->sendCMD("AT+QMTOPEN=0,8.210.84.24,1880", 1500);

      this->sendCMD("AT+QMTCONN=0," + this->IMEI + ",tswh,1Wo=[6vA0m", 3000);

      this->sendCMD("AT+QMTSUB=0,1,\"rgt/" + this->IMEI + "/in\",2", 3000);

      if (this->isOK()) {
        break;
      }

      this->sendCMD("AT+QMTUNS=0,1,\"rgt/" + this->IMEI + "/in\"", 1500);
      this->sendCMD("AT+QMTDISC=0", 1500);
      this->sendCMD("AT+QMTCLOSE=0", 1500);
    }
  }

  void checkReconnect() {
    unsigned long currMillis = millis();
    unsigned long interval = 30UL * 60UL * 1000UL;  // 30*60*1000

    Serial.println("Diff : " + String(currMillis - this->prevMillis) + " , " + String(interval) + " , " + String((currMillis - this->prevMillis) > interval));

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

    // handle receive data
    if (NBIoT_Serial.available()) {
      this->response = this->readRes();
      Serial.println(this->response);

      int isReceiving = this->response.indexOf("+QMTRECV:");

      if (isReceiving > -1) {
        int isActived = this->response.indexOf("{\"din\":1}");
        Serial.println(this->response + String(isActived));
        warningSystem.setIsActived(isActived > -1);
      }

      this->clearBuffer();
    }
  }

  ~NBIoT(){};
};

extern NBIoT nbiot;

#endif