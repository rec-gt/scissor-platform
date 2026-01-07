#include "Globals.h"
#include "AsyncTimer.h"

#ifndef IOT_H
#define IOT_H

AsyncTimer iotTimer(1000);

class IoT {
private:
  uint16_t prevMillis1 = millis();

  void listen() {
    while (SerialIoT.available() > 0) {
      char c = SerialIoT.read();
      iotSerialRecv += c;
    }
  }

  void consume() {
    int delimiterIndex = 0;
    {
      delimiterIndex = iotSerialRecv.indexOf(F("\r"));
      if (delimiterIndex > -1) {
        delimiterIndex = iotSerialRecv.indexOf(F("\n"));
      }
    }
    if (delimiterIndex != -1) {
      {
        iotExtractedRecv = iotSerialRecv.substring(0, delimiterIndex);
      }
      {
        iotSerialRecv = iotSerialRecv.substring(delimiterIndex + 1);
      }
    } else {
      iotExtractedRecv = F("");
    }
  }

  void monitorParameters() {
    // === IMEI ===
    {
      iotCmpStr = F("+CGSN: ");
      iotCmpStrIdx = iotExtractedRecv.indexOf(iotCmpStr);

      if (iotCmpStrIdx > -1) {
        {
          iotIMEI = iotExtractedRecv.substring(8, 8 + 15);
        }

        mqttConnCmd = F("AT+QMTCONN=0,dev_");
        mqttConnCmd.concat(iotIMEI);
        mqttConnCmd.concat(F(",tswh,1Wo=[6vA0m"));

        mqttSubsCmd = F("AT+QMTSUB=0,1,rgt/");
        mqttSubsCmd.concat(iotIMEI);
        mqttSubsCmd.concat(F("/out,0"));
      }
    }

    // === CSQ ===
    {
      iotCmpStr = F("+CSQ: ");
      iotCmpStrIdx = iotExtractedRecv.indexOf(iotCmpStr);
      if (iotCmpStrIdx > -1) {
        Serial.println(F("\r\n>>> CSQ RECEIVED"));
        int ws = iotExtractedRecv.indexOf(F(": "));
        int we = iotExtractedRecv.indexOf(F(","));
        {
          iotCSQ = iotExtractedRecv.substring(ws + 2, we);
        }
      }
    }

    // === CGATT ===
    {
      iotCmpStr = F("+CGATT: ");
      iotCmpStrIdx = iotExtractedRecv.indexOf(iotCmpStr);
      if (iotCmpStrIdx > -1) {
        {
          iotCGATT = iotExtractedRecv.substring(8, 9);
        }
        if (iotCGATT == F("1")) {
          if (iotConnState == IOT_STATE_WAITING_CGATT) {
            iotConnState = IOT_STATE_FINISH_CGATT;
          }
        }
      }
    }

    // === CEREG ===
    {
      iotCmpStr = F("+CEREG: ");
      iotCmpStrIdx = iotExtractedRecv.indexOf(iotCmpStr);
      if (iotCmpStrIdx > -1) {
        {
          iotCEREG = iotExtractedRecv.substring(8, 11);
        }

        if (iotCEREG == F("0,1")) {
          if (iotConnState == IOT_STATE_WAITING_CEREG) {
            iotConnState = IOT_STATE_FINISH_CEREG;
          }
        }
      }
    }
  }

  void stateManagement() {
    if (iotConnState == IOT_STATE_WAITING_INIT) {
      Serial.println(">>> INIT, RESET");
      iotConnState = IOT_STATE_WAITING_RESET;
      iotConnState = IOT_STATE_WAITING_RESET_HARDWARE;
    }

    if (iotConnState == IOT_STATE_WAITING_RESET_HARDWARE) {
      digitalWrite(24, LOW);
      if (iotTimer.asyncDelay(1000)) {
        digitalWrite(24, HIGH);
        iotConnState = IOT_STATE_FINISH_RESET_HARDWARE;
      }
    }

    if (iotConnState == IOT_STATE_FINISH_RESET_HARDWARE) {
      if (iotTimer.asyncDelay(1000)) {
        this->printlnFlush(F("AT+CFUN=1,1"));
        iotConnState = IOT_STATE_WAITING_RESET_SOFTWARE;
      }
    }

    if (iotConnState == IOT_STATE_WAITING_RESET_SOFTWARE) {
      iotCmpStr = F("RDY");
      iotCmpStrIdx = iotExtractedRecv.indexOf(iotCmpStr);
      if (iotCmpStrIdx > -1) {
        Serial.println(F("\r\n>>> IOT READY"));
        iotConnState = IOT_STATE_FINISH_RESET_SOFTWARE;
        iotConnState = IOT_STATE_FINISH_RESET;
        this->printlnFlush(F("ATI"));
      }
    }

    if (iotConnState == IOT_STATE_FINISH_RESET) {
      iotConnState = IOT_STATE_WAITING_CONFIG;
    }

    if (iotConnState == IOT_STATE_WAITING_CONFIG) {
      if (iotTimer.asyncDelay(500)) {
        this->printlnFlush(F("AT+CGSN=1"));
        this->printlnFlush(F("AT+QSCLK=0"));
        this->printlnFlush(F("AT+QIDNSCFG=0,223.5.5.5,8.8.8.8"));
        this->printlnFlush(F("AT+CFUN=1"));
        this->printlnFlush(F("AT+QSCLK=0"));
        // this->printlnFlush(F("AT+CPSMS=0")); // for nbiot
        // this->printlnFlush(F("AT+CSCON=0")); // for nbiot
        // this->printlnFlush(F("AT+CEDRXS=0,5")); // for nbiot
        this->printlnFlush(F("AT+QMTCLOSE=0"));
        this->printlnFlush(F("AT+QMTDISC=0"));

        iotConnState = IOT_STATE_FINISH_CONFIG;
      }
    }

    if (iotConnState == IOT_STATE_FINISH_CONFIG) {
      if (iotTimer.asyncDelay(500)) {
        this->printlnFlush(F("AT+CSQ"));
        iotConnState = IOT_STATE_WAITING_CSQ;
      }
    }

    if (iotConnState == IOT_STATE_WAITING_CSQ) {
      iotConnState = IOT_STATE_FINISH_CSQ;
    }

    if (iotConnState == IOT_STATE_FINISH_CSQ) {
      iotConnState = IOT_STATE_WAITING_CGATT;
    }

    if (iotConnState == IOT_STATE_WAITING_CGATT) {
      if (iotTimer.asyncDelay(1000)) {
        this->printlnFlush(F("AT+CGATT?"));
      }
    }

    if (iotConnState == IOT_STATE_FINISH_CGATT) {
      iotConnState = IOT_STATE_WAITING_CEREG;
    }

    if (iotConnState == IOT_STATE_WAITING_CEREG) {
      if (iotTimer.asyncDelay(1000)) {
        this->printlnFlush(F("AT+CEREG?"));
      }
    }

    if (iotConnState == IOT_STATE_FINISH_CEREG) {
      iotConnState = IOT_STATE_WAITING_OPEN_MQTT;
    }

    if (iotConnState == IOT_STATE_WAITING_OPEN_MQTT) {
      if (iotTimer.asyncDelay(1000)) {
        this->printlnFlush(F("AT+QMTOPEN=0,iot.rec-gt.com,1880"));
      }

      iotCmpStr = F("+QMTOPEN: 0,0");
      iotCmpStrIdx = iotExtractedRecv.indexOf(iotCmpStr);
      if (iotCmpStrIdx > -1) {
        iotConnState = IOT_STATE_FINISH_OPEN_MQTT;
      }
    }

    if (iotConnState == IOT_STATE_FINISH_OPEN_MQTT) {
      iotConnState = IOT_STATE_WAITING_CONN_MQTT;
    }

    if (iotConnState == IOT_STATE_WAITING_CONN_MQTT) {
      if (iotTimer.asyncDelay(1000)) {
        this->printlnFlush(mqttConnCmd);
      }

      iotCmpStr = F("+QMTCONN: 0,0,0");
      iotCmpStrIdx = iotExtractedRecv.indexOf(iotCmpStr);
      if (iotCmpStrIdx > -1) {
        iotConnState = IOT_STATE_FINISH_CONN_MQTT;
      }
    }

    if (iotConnState == IOT_STATE_FINISH_CONN_MQTT) {
      iotConnState = IOT_STATE_WAITING_SUBS_MQTT_TOPIC;
    }

    if (iotConnState == IOT_STATE_WAITING_SUBS_MQTT_TOPIC) {
      if (iotTimer.asyncDelay(1000)) {
        this->printlnFlush(mqttSubsCmd);
      }

      iotCmpStr = F("+QMTSUB: ");
      iotCmpStrIdx = iotExtractedRecv.indexOf(iotCmpStr);
      if (iotCmpStrIdx > -1) {
        iotConnState = IOT_STATE_FINISH_SUBS_MQTT_TOPIC;
      }
    }
  }

public:
  void init() {
    SerialIoT.begin(115200);
    pinMode(24, OUTPUT);
    digitalWrite(24, HIGH);
    iotConnState = IOT_STATE_WAITING_INIT;
  }

  void loop() {
    this->listen();
    this->consume();
    this->stateManagement();
    this->monitorParameters();
    this->printExtractedRecv();
  }

  void printlnFlush(const String& cmd) {
    SerialIoT.println(cmd);
    SerialIoT.flush();
    delay(1);
  }

  void printExtractedRecv() {
    if (iotExtractedRecv != F("")) {
      Serial.print("[[");
      Serial.print(iotExtractedRecv);
      Serial.println("]]");
    }
  }

  void printSerialRecv() {
    Serial.print("[[");
    Serial.print(iotSerialRecv);
    Serial.println("]]");
  }

  void printParameterState() {
    if (iotTimer.asyncDelay(1000)) {
      Serial.println("iotCSQ: " + iotCSQ);
      Serial.println("iotIMEI: " + iotIMEI);
      Serial.println("iotCGATT: " + iotCGATT);
      Serial.println("iotCEREG: " + iotCEREG);
    }
  }
};

extern IoT iot;

#endif

// A  T  +  C  S  Q  +  C  S  Q  :  _  2  4  ,  9  9  O  K
// 41 54 2B 43 53 51 2B 43 53 51 3A 20 32 34 2C 39 39 4F 4B 0
