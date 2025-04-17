#include "DetectSystem.h"
#include "DisplayOLED.h"

#ifndef NBIoT_h
#define NBIoT_h
#define NBIoTModule Serial1

class NBIoT {
private:
  byte errCount = 0;
  byte RN = 2;
  String response;

  void clearBuffer() {
    while (NBIoTModule.read() > 0) { delay(1); }
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

    if (this->errCount >= 15) {
      if (this->CSQ.toInt() > 30) {
        displayOLED.print("", "IoT SIM Card", "Not Found", 401);
      } else {
        displayOLED.print("", "IoT SIM Card", "Not Found", 402);
      }
      Serial.println("MQTT init failed");
      while (1) {};
    }
  }

public:
  String CIMI;
  byte CIMIErrCnt = 0;

  String CSQ;
  byte CSQErrCnt = 0;

  String IMEI;

  NBIoT(){};

  bool sendCMD(String cmd) {
    this->clearBuffer();
    Serial.println("CMD: " + cmd);
    NBIoTModule.println(cmd);
    delay(500);  // wait at least 300ms

    if (NBIoTModule.available()) {
      this->response = NBIoTModule.readString();
      Serial.println(this->response);
      this->clearBuffer();
      return true;
    } else {
      this->clearBuffer();
      return false;
    }
  }

  void sendCMDFast(String cmd) {
    this->clearBuffer();
    Serial.println("CMD: " + cmd);
    NBIoTModule.println(cmd);
    delay(300);
  }

  bool init() {
    displayOLED.print("", "正在加載IoT...", "", 2);

    delay(100);

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
      this->sendCMD("AT+MQTTOPEN=1,1,1,0,1,\"rgt/" + this->IMEI + "/in\",\"gone\"");
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

  ~NBIoT(){};
};

extern NBIoT nbiot;

#endif