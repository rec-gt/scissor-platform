#include "Globals.h"

#ifndef IOTS_H
#define IOTS_H

class IoT {
private:
  void listen() {
    while (SerialIoT.available() > 0) {
      char c = SerialIoT.read();
      iotSerialRecv += c;
    }
  }

  void consume() {
    int delimiterIndex = -1;

    {  // boundary protection
      delimiterIndex = iotSerialRecv.length();
    }

    {  // delimiter check
      delimiterIndex = iotSerialRecv.indexOf(F("\r"));
      if (delimiterIndex > -1) {
        delimiterIndex = iotSerialRecv.indexOf(F("\n"));
      }
    }

    if (delimiterIndex > -1) {
      {
        iotExtractedRecv = iotSerialRecv.substring(0, delimiterIndex);
        if (iotExtractedRecv == F("\r") || iotExtractedRecv == F("\n") || iotExtractedRecv == F("\r\n") || iotExtractedRecv == F("\n\r")) {
          iotExtractedRecv = F("");
        }
      }
      {
        iotSerialRecv = iotSerialRecv.substring(delimiterIndex + 1);
      }
    } else {
      iotExtractedRecv = F("");
    }
  }

  void manageModuleState() {
    if (iotModuleState == IOT_MODULE_WAITING_INIT) {
      Serial.println(F(">>> IOT MODULE INIT, RESET"));
      iotModuleState = IOT_MODULE_WAITING_RESET;
      iotModuleState = IOT_MODULE_WAITING_RESET_HARDWARE;
      iotConnState = IOT_CONN_WAITING_INIT;

      iotCSQErrCnt.reset();
      iotCGATTErrCnt.reset();
      iotCEREGErrCnt.reset();
      mqttOpenErrCnt.reset();
      mqttConnErrCnt.reset();
      mqttSubsErrCnt.reset();
      mqttPublErrCnt.reset();

      mqttPublLock.release();
      mqttForcePublMode.off();
    }

    if (iotModuleState == IOT_MODULE_WAITING_RESET_HARDWARE) {
      digitalWrite(IOT_MODULE_RESET_PIN, LOW);
      if (iotModuleStateTimer.autoTimeout(2000)) {
        digitalWrite(IOT_MODULE_RESET_PIN, HIGH);
        iotModuleState = IOT_MODULE_FINISH_RESET_HARDWARE;
      }
    }

    if (iotModuleState == IOT_MODULE_FINISH_RESET_HARDWARE) {
      if (iotModuleStateTimer.autoTimeout(1000)) {
        this->printlnFlush(F("AT+CFUN=1,1"));
        iotModuleState = IOT_MODULE_WAITING_RESET_SOFTWARE;
        iotModuleState = IOT_MODULE_FINISH_RESET;
      }
    }

    if (iotModuleState == IOT_MODULE_FINISH_RESET) {
      if (iotExtractedRecv.indexOf(F("RDY")) > -1) {
        this->printlnFlush(F("ATI"));
        iotModuleState = IOT_MODULE_WAITING_GET_MODEL;
        iotSoftWatchdog.pet();
      }
    }

    if (iotModuleState == IOT_MODULE_WAITING_GET_MODEL) {
      bool res = false;

      if (iotExtractedRecv.indexOf(IOT_MODEL_EC800K) > -1) {
        iotModel = IOT_MODEL_EC800K;
        res = true;
      } else if (iotExtractedRecv.indexOf(IOT_MODEL_BC260Y_CN) > -1) {
        iotModel = IOT_MODEL_BC260Y_CN;
        res = true;
      }

      if (res) {
        iotModuleState = IOT_MODULE_FINISH_GET_MODEL;
        iotModuleState = IOT_MODULE_FINISH_INIT;
        iotSoftWatchdog.pet();
      }
    }

    if (iotModuleState == IOT_MODULE_FINISH_INIT) {
      iotConnState = IOT_CONN_WAITING_INIT;
      iotModuleState = IOT_MODULE_END_OF_STATE;
    }
  }

  void handleMQTTSubs() {
    if (iotExtractedRecv.indexOf(F("+QMTRECV: ")) > -1) {
      {
        mqttSubsMsgContent = iotExtractedRecv.substring(41, 46);
        Serial.println(mqttSubsMsgContent);
      }
    }
  }

protected:
  void manageConnectionState() {
    if (iotConnState == IOT_CONN_WAITING_INIT) {
      Serial.println(F(">>> IOT CONNECTION INIT"));
      iotConnState = IOT_CONN_WAITING_CONFIG;
    }

    if (iotConnState == IOT_CONN_WAITING_CONFIG) {
      if (iotStateTimer.autoTimeout(500)) {
        this->printlnFlush(F("ATE0"));
        this->printlnFlush(F("AT+CGSN=1"));
        this->printlnFlush(F("AT+QSCLK=0"));
        this->printlnFlush(F("AT+QIDNSCFG=0,223.5.5.5,8.8.8.8"));
        this->printlnFlush(F("AT+CFUN=1"));
        // this->printlnFlush(F("AT+CPSMS=0")); // for nbiot
        // this->printlnFlush(F("AT+CSCON=0")); // for nbiot
        // this->printlnFlush(F("AT+CEDRXS=0,5")); // for nbiot
        this->printlnFlush(F("AT+QMTCLOSE=0"));
        this->printlnFlush(F("AT+QMTDISC=0"));

        iotConnState = IOT_CONN_FINISH_CONFIG;

        iotSoftWatchdog.pet();
      }
    }

    if (iotConnState == IOT_CONN_FINISH_CONFIG) {
      if (iotStateTimer.autoTimeout(500)) {
        this->printlnFlush(F("AT+CSQ"));
        iotConnState = IOT_CONN_WAITING_CSQ;
      }
    }

    if (iotConnState == IOT_CONN_WAITING_CSQ) {
      iotConnState = IOT_CONN_FINISH_CSQ;
    }

    if (iotConnState == IOT_CONN_FINISH_CSQ) {
      iotConnState = IOT_CONN_WAITING_CGATT;
    }

    if (iotConnState == IOT_CONN_WAITING_CGATT) {
      if (iotStateTimer.autoTimeout(1000)) {
        this->printlnFlush(F("AT+CGATT?"));
      }
    }

    if (iotConnState == IOT_CONN_FINISH_CGATT) {
      iotConnState = IOT_CONN_WAITING_CEREG;
    }

    if (iotConnState == IOT_CONN_WAITING_CEREG) {
      if (iotStateTimer.autoTimeout(1000)) {
        this->printlnFlush(F("AT+CEREG?"));
      }
    }

    if (iotConnState == IOT_CONN_FINISH_CEREG) {
      if (iotStateTimer.autoTimeout(1000)) {
        this->printlnFlush(F("AT+QMTOPEN=0,iot.rec-gt.com,1880"));
        iotConnState = IOT_CONN_WAITING_OPEN_MQTT;
      }
    }

    if (iotConnState == IOT_CONN_WAITING_OPEN_MQTT) {
      if (iotExtractedRecv.indexOf(F("+QMTOPEN: 0,0")) > -1) {
        iotConnState = IOT_CONN_FINISH_OPEN_MQTT;
        mqttOpenErrCnt.reset();
        iotSoftWatchdog.pet();
      } else {
        if (mqttStateTimer.autoTimeout(1000)) {
          mqttOpenErrCnt.accu();
        }
      }
    }

    if (iotConnState == IOT_CONN_FINISH_OPEN_MQTT) {
      if (iotStateTimer.autoTimeout(1000)) {
        this->printlnFlush(mqttConnCmd);
        iotConnState = IOT_CONN_WAITING_CONN_MQTT;
      }
    }

    if (iotConnState == IOT_CONN_WAITING_CONN_MQTT) {
      if (iotExtractedRecv.indexOf(F("+QMTCONN: 0,0,0")) > -1) {
        iotConnState = IOT_CONN_FINISH_CONN_MQTT;
        mqttConnErrCnt.reset();
        iotSoftWatchdog.pet();
      } else {
        if (mqttStateTimer.autoTimeout(1000)) {
          mqttConnErrCnt.accu();
        }
      }
    }

    if (iotConnState == IOT_CONN_FINISH_CONN_MQTT) {
      if (iotStateTimer.autoTimeout(1000)) {
        this->printlnFlush(mqttSubsCmd);
        iotConnState = IOT_CONN_WAITING_SUBS_MQTT_TOPIC;
      }
    }

    if (iotConnState == IOT_CONN_WAITING_SUBS_MQTT_TOPIC) {
      if (iotExtractedRecv.indexOf(F("+QMTSUB: 0,1,0,0")) > -1) {
        iotConnState = IOT_CONN_FINISH_SUBS_MQTT_TOPIC;
        iotConnState = IOT_CONN_FINISH_INIT;
        mqttSubsErrCnt.reset();
        iotSoftWatchdog.pet();
      } else {
        if (mqttStateTimer.autoTimeout(1000)) {
          mqttSubsErrCnt.accu();
        }
      }
    }

    if (iotConnState == IOT_CONN_FINISH_INIT) {
      iotConnState = MQTT_STATE_INIT;
    }

    if (iotConnState == MQTT_STATE_INIT) {
      if (mqttForcePublMode.isOn()) {
        mqttPublLock.lock();
        this->printlnFlush(mqttPublMsgPrepare);
        Serial.println(mqttPublMsgPrepare);
        iotConnState = MQTT_STATE_WAITING_PREPARE_PUBMSG;
        mqttForcePublMode.off();
      } else {
        if (iotStateTimer.autoTimeout(30000)) {
          mqttPublLock.lock();
          this->printlnFlush(mqttPublMsgPrepare);
          Serial.println(mqttPublMsgPrepare);
          iotConnState = MQTT_STATE_WAITING_PREPARE_PUBMSG;
        }
      }
    }

    if (iotConnState == MQTT_STATE_WAITING_PREPARE_PUBMSG) {
      if (iotSerialRecv.indexOf(F("> ")) > -1) {
        iotConnState = MQTT_STATE_FINISH_PREPARE_PUBMSG;
        mqttPublErrCnt.reset();
      } else {
        if (iotStateTimer.autoTimeout(1000)) {
          mqttPublErrCnt.accu();
        }
      }
    }

    if (iotConnState == MQTT_STATE_FINISH_PREPARE_PUBMSG) {
      Serial.println(mqttPublMsgPayload);
      this->printlnFlush(mqttPublMsgPayload);
      mqttPublLock.release();
      iotConnState = MQTT_STATE_WAITING_PUBLISH;
      iotConnState = MQTT_STATE_INIT;  // loop-back
      iotSoftWatchdog.pet();
    }
  }

  void captureIMEI() {
    if (iotExtractedRecv.indexOf(F("+CGSN: ")) > -1) {

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

  void captureIP() {
    if (iotExtractedRecv.indexOf(F("+IP:")) > -1) {
      Serial.println(iotExtractedRecv);
      iotIP = F("Has IP");
    }
  }

  void captureCSQ() {
    if (iotExtractedRecv.indexOf(F("+CSQ: ")) > -1) {
      int ws = iotExtractedRecv.indexOf(F(": "));
      int we = iotExtractedRecv.indexOf(F(","));

      {
        iotCSQ = iotExtractedRecv.substring(ws + 2, we);
      }
    }
  }

  void captureCGATT() {
    if (iotExtractedRecv.indexOf(F("+CGATT: ")) > -1) {
      iotCGATT = iotExtractedRecv.substring(8, 9);
    }
  }

  void captureCEREG() {
    if (iotExtractedRecv.indexOf(F("+CEREG: ")) > -1) {
      iotCEREG = iotExtractedRecv.substring(8, 11);
    }
  }

  void inspectIMEI() {
    //
  }

  void inspectIP() {
    if (iotIP != F("")) {
      if (iotConnState == IOT_CONN_WAITING_ASSIGN_IP) {
        iotConnState = IOT_CONN_FINISH_ASSIGN_IP;
        iotSoftWatchdog.pet();
      }
    }
  }

  void inspectCSQ() {
    byte CSQReading = 0;

    {
      CSQReading = iotCSQ.toInt();
    }

    if (CSQReading == 99 || CSQReading <= 3) {
      // iotCSQErrCnt.accu();
    } else {
      // iotCSQErrCnt.reset();
    }
  }

  void inspectCGATT() {
    if (iotCGATT == F("1")) {
      if (iotConnState == IOT_CONN_WAITING_CGATT) {
        iotConnState = IOT_CONN_FINISH_CGATT;
        iotSoftWatchdog.pet();
      }
      // iotCGATTErrCnt.reset();
    } else {
      // iotCGATTErrCnt.accu();
    }
  }

  void inspectCEREG() {
    if (iotCEREG == F("0,1")) {
      if (iotConnState == IOT_CONN_WAITING_CEREG) {
        iotConnState = IOT_CONN_FINISH_CEREG;
        iotSoftWatchdog.pet();
      }
      // iotCEREGErrCnt.reset();
    } else {
      // iotCEREGErrCnt.accu();
    }
  }

  void queryParams() {
    if (mqttPublLock.isReleased()) {
      if (iotParamTimer.autoTimeout(5000)) {
        Serial.println(F(">>> Query?"));
        this->printlnFlush(F("AT+CPIN?"));
        this->printlnFlush(F("AT+CSQ"));
        this->printlnFlush(F("AT+CGATT?"));
        this->printlnFlush(F("AT+CEREG?"));
      }
    }
  }

  void errHook() {
    if (iotExtractedRecv.indexOf(F("+QIURC: \"pdpdeact\",1")) > -1) {
      iotModuleState = IOT_MODULE_WAITING_INIT;
    }

    if (iotExtractedRecv.indexOf(F("+CME ERROR")) > -1) {
      iotModuleState = IOT_MODULE_WAITING_INIT;
    }

    if (iotCSQErrCnt.over(10)) {
      Serial.print(F("iotCSQErrCnt.over(10)"));
      iotModuleState = IOT_MODULE_WAITING_INIT;
    }

    if (iotCGATTErrCnt.over(10)) {
      Serial.print(F("iotCGATTErrCnt.over(10)"));
      iotModuleState = IOT_MODULE_WAITING_INIT;
    }

    if (iotCEREGErrCnt.over(10)) {
      Serial.print(F("iotCEREGErrCnt.over(10)"));
      iotModuleState = IOT_MODULE_WAITING_INIT;
    }

    if (mqttOpenErrCnt.over(5)) {
      Serial.print(F("mqttOpenErrCnt.over(5)"));
      iotModuleState = IOT_MODULE_WAITING_INIT;
    }

    if (mqttConnErrCnt.over(5)) {
      Serial.print(F("mqttConnErrCnt.over(5)"));
      iotModuleState = IOT_MODULE_WAITING_INIT;
    }

    if (mqttSubsErrCnt.over(5)) {
      Serial.print(F("mqttSubsErrCnt.over(5)"));
      iotModuleState = IOT_MODULE_WAITING_INIT;
    }

    if (mqttPublErrCnt.over(5)) {
      Serial.print(F("mqttPublErrCnt.over(5)"));
      iotModuleState = IOT_MODULE_WAITING_INIT;
    }
  }

public:
  void init() {
    SerialIoT.begin(115200);
    pinMode(IOT_MODULE_RESET_PIN, OUTPUT);
    digitalWrite(IOT_MODULE_RESET_PIN, HIGH);
    iotModuleState = IOT_MODULE_WAITING_INIT;

    /* === Soft Watchdog for IoT Service=== */
    iotSoftWatchdog.enable();
    iotSoftWatchdog.setCallback([]() {
      iotModuleState = IOT_MODULE_WAITING_INIT;
      Serial.println(F(">>> IoT Watchdog Awaken"));
    });
  }

  void loop() {
    iotSoftWatchdog.monitor();

    this->listen();
    this->consume();
    this->manageModuleState();
    this->manageConnectionState();

    this->queryParams();

    this->captureIMEI();
    this->captureIP();
    this->captureCSQ();
    this->captureCGATT();
    this->captureCEREG();

    this->inspectIMEI();
    this->inspectIP();
    this->inspectCSQ();
    this->inspectCGATT();
    this->inspectCEREG();

    this->handleMQTTSubs();
    this->errHook();

    this->printExtractedRecv();
  }

  void printlnFlush(const String& cmd) {
    SerialIoT.println(cmd);
    SerialIoT.flush();
    delay(1);
  }

  void buildMsg(byte _DIPayload, byte _DOPayload, const String& _AIPayload, const String& _AOPayload) {
    // 1. build payload
    mqttPublMsgPayload = F("{\"csq\":");
    mqttPublMsgPayload.concat(iotCSQ);
    mqttPublMsgPayload.concat(F(","));
    mqttPublMsgPayload.concat(F("\"din\":"));
    mqttPublMsgPayload.concat(_DIPayload);
    mqttPublMsgPayload.concat(F(","));
    mqttPublMsgPayload.concat(F("\"dout\":"));
    mqttPublMsgPayload.concat(_DOPayload);
    mqttPublMsgPayload.concat(F(","));
    mqttPublMsgPayload.concat(F("\"ain\":"));
    mqttPublMsgPayload.concat(_AIPayload);
    mqttPublMsgPayload.concat(F(","));
    mqttPublMsgPayload.concat(F("\"aout\":"));
    mqttPublMsgPayload.concat(_AOPayload);
    mqttPublMsgPayload.concat(F("}"));

    // 2. build prepare msg
    mqttPublMsgPrepare = F("AT+QMTPUBEX=0,0,0,0,rgt/");
    mqttPublMsgPrepare.concat(iotIMEI);
    mqttPublMsgPrepare.concat(F("/in,"));
    mqttPublMsgPrepare.concat(mqttPublMsgPayload.length());
  }

  void forcePublish() {
    if (MQTT_STATE_INIT <= iotConnState && iotConnState <= MQTT_STATE_FINISH_PUBLISH) {
      mqttForcePublMode.on();
    }
  }

  void printExtractedRecv() {
    if (iotExtractedRecv != F("")) {
      Serial.print(F("[["));
      Serial.print(iotExtractedRecv);
      Serial.println(F("]]"));
    }
  }

  void printSerialRecv() {
    Serial.print(F("[["));
    Serial.print(iotSerialRecv);
    Serial.println(F("]]"));
  }
};

extern IoT iot;

#endif

// A  T  +  C  S  Q  +  C  S  Q  :  _  2  4  ,  9  9  O  K
// 41 54 2B 43 53 51 2B 43 53 51 3A 20 32 34 2C 39 39 4F 4B 0