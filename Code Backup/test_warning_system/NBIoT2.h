#include <ArduinoJson.h>
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
  String resCode;

  NBIoT(){};

  bool sendCMD(String cmd) {
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
      if (this->resContain("+CSQ") && !this->resContain("ERROR")) {
        this->parseCSQ();
        char* csq_c = this->CSQ.c_str();
        if (this->resContain("99,99") || !utils.isNumeric(csq_c)) {
          this->errHook(true);
          delay(1000);
          continue;
        } else {
          break;
        }
      } else {
        this->errHook(true);
        delay(1000);
      }
    }

    while (1) {
      this->sendCMD("AT+CEREG?");
      if (this->resContain("+CEREG:") && !this->resContain("ERROR")) {
        break;
      } else {
        this->errHook(true);
        delay(1000);
      }
    }

    while (1) {
      this->sendCMD("AT+CGATT?");
      if (this->resContain("+CGATT:1") && !this->resContain("ERROR")) {
        break;
      } else {
        this->errHook(true);
        delay(1000);
      }
    }

    // get cimi
    while (1) {
      this->sendCMD("AT+CIMI");
      if (this->resContain("+CIMI:") && !this->resContain("ERROR")) {
        this->parseCIMI();
        break;
      } else {
        this->errHook(true);
        delay(1000);
      }
    }

    while (1) {
      this->sendCMD("AT+CGSN=1");
      if (this->resContain("+CGSN:") && !this->resContain("ERROR")) {
        this->parseIMEI();
        break;
      } else {
        this->errHook(true);
        delay(1000);
      }
    }


    while (1) {
      this->sendCMD("AT+MQTTREC=0");
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
      if (this->resContain("OK") && !this->resContain("ERROR")) {
        delay(3000);
        while (NBIoT_Serial.available()) {
          this->response = NBIoT_Serial.readString();
          if (this->resContain("+MQTTSUBACK")) {
            this->clearBuffer();
            break;
          }
        }
        break;
      } else {
        this->errHook(true);
        delay(1000);
      }
    }

    Serial.println("MQTT Init Finished");
  }

  void listen() {
    NBIoT_Serial.println("AT+MQTTSUB?");
    delay(300);  // according to docs, wait at least 300ms
    if (NBIoT_Serial.available()) {
      this->response = NBIoT_Serial.readString();
      if (!this->resContain("TOPIC 123")) {
        NBIoT_Serial.println("AT+MQTTSUB=TOPIC 123,0,0");
      }else{

      }
    }

    if (NBIoT_Serial.available()) {
      this->response = NBIoT_Serial.readString();
      Serial.println(this->response);

      String jsonStr = utils.retrieveMsg(this->response);

      DynamicJsonDocument doc(1024);
      DeserializationError error = deserializeJson(doc, jsonStr);

      if (error) {
        Serial.print("Parsing failed: ");
        Serial.println(error.c_str());
      } else {
        JsonObject msg = doc["msg"];
        String code = msg["code"];
        this->resCode = code;
        Serial.println(this->resCode);
      }
      // }

      this->clearBuffer();
    }
  }

  ~NBIoT(){};
};

extern NBIoT nbiot;

#endif