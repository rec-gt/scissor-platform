#include "Globals.h"

#ifndef IOTS_H
#define IOTS_H

class IoT {
private:
  void listen() {
    while (SerialIoT.available() > 0) {
      char c = SerialIoT.read();
      Serial.print(c);
      iotSerialRecv += c;
    }
  }

  void clearRecv() {
    iotSerialRecv = F("");
  }

  void manageModuleState() {
    if (iotModuleState == IOT_MODULE_WAITING_INIT) {
      Serial.println(F(">>> IOT MODULE INIT, RESET"));
      iotModuleState = IOT_MODULE_WAITING_RESET;
      iotConnState = IOT_CONN_WAITING_INIT;

      iotSerialRecv = F("");
      iotSerialRecv = F("");

      iotModel = F("");
      iotIMEI = F("");
      iotCSQ = F("");
      iotCGATT = F("");
      iotCEREG = F("");

      mqttConnCmd = F("");
      mqttSubsCmd = F("");

      mqttPublLock.release();
      mqttForcePublMode.off();

      mqttPublMsgPrepare = F("");
      mqttPublMsgPayload = F("");
      mqttSubsMsgContent = F("");

      iotCSQErrCnt.reset();
      iotCGATTErrCnt.reset();
      iotCEREGErrCnt.reset();
      mqttOpenErrCnt.reset();
      mqttConnErrCnt.reset();
      mqttSubsErrCnt.reset();
      mqttPublErrCnt.reset();
    }

    if (iotModuleState == IOT_MODULE_WAITING_RESET) {
      SerialIoT.begin(iotSerialBaudRates[iotSerialBaudRateIdx]);

      digitalWrite(IOT_MODULE_RESET_PIN, LOW);
      if (iotModuleStateTimer.autoTimeout(2000)) {
        digitalWrite(IOT_MODULE_RESET_PIN, HIGH);
        iotModuleState = IOT_MODULE_WAITING_RESET_HARDWARE;
      }
    }

    if (iotModuleState == IOT_MODULE_WAITING_RESET_HARDWARE) {
      this->listen();
      if (iotSerialRecv.indexOf(F("RDY")) > -1) {
        Serial.println(F(">>> FINISH HARDWARE RESET"));
        this->clearRecv();
        this->printlnFlush(F("ATE1"));
        this->printlnFlush(F("AT+QSCLK=0"));
        iotModuleState = IOT_MODULE_FINISH_RESET_HARDWARE;

        iotResetHardwareCnt.reset();
      } else {
        if (iotModuleStateTimer.autoTimeout(1000)) {
          Serial.print(F(">>> IOT SERIAL PROBE... ("));
          Serial.print(iotSerialBaudRates[iotSerialBaudRateIdx]);
          Serial.println(F(")"));
          iotResetHardwareCnt.accu();
        }

        if (iotResetHardwareCnt.over(5)) {
          Serial.println(F(">>> IOT SERIAL FAIL!"));
          iotSerialBaudRateIdx++;
          if (iotSerialBaudRateIdx > 1) {
            iotSerialBaudRateIdx = 0;
          }
          iotModuleState = IOT_MODULE_WAITING_INIT;
        }
      }
    }

    if (iotModuleState == IOT_MODULE_FINISH_RESET_HARDWARE) {
      iotModuleState = IOT_MODULE_FINISH_RESET_SOFTWARE;
      iotModuleState = IOT_MODULE_FINISH_RESET;
    }

    if (iotModuleState == IOT_MODULE_FINISH_RESET) {
      if (iotModuleStateTimer.autoTimeout(1000)) {
        this->printlnFlush(F("ATI"));
        iotModuleState = IOT_MODULE_WAITING_GET_MODEL;
      }
    }


    if (iotModuleState == IOT_MODULE_WAITING_GET_MODEL) {
      this->listen();

      bool res = false;

      if (iotSerialRecv.indexOf(IOT_MODEL_EC800K) > -1) {
        iotModel = IOT_MODEL_EC800K;
        res = true;
      } else if (iotSerialRecv.indexOf(IOT_MODEL_BC260Y_CN) > -1) {
        iotModel = IOT_MODEL_BC260Y_CN;
        res = true;
      }

      if (res) {
        Serial.print(iotModel);
        this->clearRecv();
        iotModuleState = IOT_MODULE_FINISH_GET_MODEL;
        iotModuleState = IOT_MODULE_FINISH_INIT;
        iotSoftWatchdog.pet();
      }
    }

    if (iotModuleState == IOT_MODULE_FINISH_INIT) {
      iotModuleState = IOT_MODULE_END_OF_STATE;
      iotConnState = IOT_CONN_WAITING_INIT;
    }
  }

  void handleMQTTSubs() {
    if (iotSerialRecv.indexOf(F("+QMTRECV: ")) > -1) {
      {
        mqttSubsMsgContent = iotSerialRecv.substring(41, 46);
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
      if (iotConnStateTimer.autoTimeout(1000)) {
        this->printlnFlush(F("AT+CFUN=1"));
        this->printlnFlush(F("AT+QSCLK=0"));
        this->printlnFlush(F("AT+QIDNSCFG=0,1.1.1.1,223.5.5.5"));

        if (iotModel == IOT_MODEL_BC260Y_CN) {
          this->printlnFlush(F("AT+CFUN=1"));
          this->printlnFlush(F("AT+QSCLK=0"));
          this->printlnFlush(F("AT+CPSMS=0"));
          this->printlnFlush(F("AT+CSCON=0"));
          this->printlnFlush(F("AT+CEDRXS=0,5"));
        }

        this->printlnFlush(F("AT+QMTCLOSE=0"));
        this->printlnFlush(F("AT+QMTDISC=0"));

        iotConnState = IOT_CONN_FINISH_CONFIG;

        iotSoftWatchdog.pet();
      }
    }

    if (iotConnState == IOT_CONN_FINISH_CONFIG) {
      if (iotConnStateTimer.autoTimeout(1000)) {
        this->printlnFlush(F("AT+CGSN=1"));
      }
      this->listen();
      this->captureIMEI();
      this->clearRecv();
      if (iotIMEI.length() == 15) {
        iotConnState = IOT_CONN_WAITING_CSQ;
      }
    }

    if (iotConnState == IOT_CONN_WAITING_CSQ) {
      if (iotConnStateTimer.autoTimeout(1000)) {
        this->printlnFlush(F("AT+CSQ"));
      }
      this->listen();
      this->captureCSQ();
      this->clearRecv();

      if (this->inspectCSQ()) {
        iotConnState = IOT_CONN_FINISH_CSQ;

        iotCSQErrCnt.reset();
        iotSoftWatchdog.pet();
      } else {
        if (iotRetryTimer.autoTimeout(1000)) {
          iotCSQErrCnt.accu();
        }
      }
    }

    if (iotConnState == IOT_CONN_FINISH_CSQ) {
      iotConnState = IOT_CONN_WAITING_CGATT;
    }

    if (iotConnState == IOT_CONN_WAITING_CGATT) {
      if (iotConnStateTimer.autoTimeout(1000)) {
        this->printlnFlush(F("AT+CGATT?"));
        Serial.println(iotSerialRecv);
      }

      this->listen();
      this->captureCGATT();
      this->clearRecv();

      if (this->inspectCGATT()) {
        iotConnState = IOT_CONN_FINISH_CGATT;

        iotCGATTErrCnt.reset();
        iotSoftWatchdog.pet();
      } else {
        if (iotRetryTimer.autoTimeout(1000)) {
          iotCGATTErrCnt.accu();
        }
      }
    }

    if (iotConnState == IOT_CONN_FINISH_CGATT) {
      iotConnState = IOT_CONN_WAITING_CEREG;
    }

    if (iotConnState == IOT_CONN_WAITING_CEREG) {
      if (iotConnStateTimer.autoTimeout(1000)) {
        this->printlnFlush(F("AT+CEREG?"));
      }

      this->listen();
      this->captureCEREG();
      this->clearRecv();

      if (this->inspectCEREG()) {
        iotConnState = IOT_CONN_FINISH_CEREG;

        iotSoftWatchdog.pet();
        iotCEREGErrCnt.reset();
      } else {
        if (iotRetryTimer.autoTimeout(1000)) {
          iotCEREGErrCnt.accu();
        }
      }
    }

    if (iotConnState == IOT_CONN_FINISH_CEREG) {
      if (iotModel == IOT_MODEL_EC800K) {
        if (iotConnStateTimer.autoTimeout(1000)) {
          this->printlnFlush(F("AT+QMTOPEN=0,iot.rec-gt.com,1880"));
          iotConnState = IOT_CONN_WAITING_OPEN_MQTT;
        }
      } else if (iotModel == IOT_MODEL_BC260Y_CN) {
        if (iotConnStateTimer.autoTimeout(1000)) {
          this->printlnFlush(F("AT+QMTOPEN=0,iot.rec-gt.com,1880"));
          // this->printlnFlush(F("AT+QMTOPEN=0,8.210.84.24,1880"));
          iotConnState = IOT_CONN_WAITING_OPEN_MQTT;
        }
      }
    }

    if (iotConnState == IOT_CONN_WAITING_OPEN_MQTT) {
      this->listen();
      if (iotSerialRecv.indexOf(F("+QMTOPEN: 0,0")) > -1) {
        this->clearRecv();
        iotConnState = IOT_CONN_FINISH_OPEN_MQTT;
        mqttOpenErrCnt.reset();
        iotSoftWatchdog.pet();
      } else {
        if (iotRetryTimer.autoTimeout(1000)) {
          mqttOpenErrCnt.accu();
        }
      }
    }

    if (iotConnState == IOT_CONN_FINISH_OPEN_MQTT) {
      if (iotConnStateTimer.autoTimeout(1000)) {
        this->printlnFlush(mqttConnCmd);
        iotConnState = IOT_CONN_WAITING_CONN_MQTT;
      }
    }

    if (iotConnState == IOT_CONN_WAITING_CONN_MQTT) {
      this->listen();
      if (iotSerialRecv.indexOf(F("+QMTCONN: 0,0,0")) > -1) {
        this->clearRecv();
        iotConnState = IOT_CONN_FINISH_CONN_MQTT;

        mqttConnErrCnt.reset();
        iotSoftWatchdog.pet();
      } else {
        if (iotRetryTimer.autoTimeout(1000)) {
          mqttConnErrCnt.accu();
        }
      }
    }

    if (iotConnState == IOT_CONN_FINISH_CONN_MQTT) {
      if (iotConnStateTimer.autoTimeout(1000)) {
        this->printlnFlush(mqttSubsCmd);
        iotConnState = IOT_CONN_WAITING_SUBS_MQTT_TOPIC;
      }
    }

    if (iotConnState == IOT_CONN_WAITING_SUBS_MQTT_TOPIC) {
      this->listen();

      if (iotSerialRecv.indexOf(F("+QMTSUB: 0,1,0,0")) > -1) {
        this->clearRecv();

        iotConnState = IOT_CONN_FINISH_SUBS_MQTT_TOPIC;
        iotConnState = IOT_CONN_FINISH_INIT;
        mqttSubsErrCnt.reset();
        iotSoftWatchdog.pet();
      } else {
        if (iotRetryTimer.autoTimeout(1000)) {
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
        iotConnState = MQTT_STATE_WAITING_PREPARE_PUBMSG;
      } else {
        if (iotConnStateTimer.autoTimeout(10000)) {
          mqttPublLock.lock();
          this->printlnFlush(mqttPublMsgPrepare);
          iotConnState = MQTT_STATE_WAITING_PREPARE_PUBMSG;
        }
      }
    }

    if (iotConnState == MQTT_STATE_WAITING_PREPARE_PUBMSG) {
      this->listen();

      if (iotSerialRecv.indexOf(F(">")) > -1) {
        this->clearRecv();
        iotConnState = MQTT_STATE_FINISH_PREPARE_PUBMSG;

        mqttPublErrCnt.reset();
      } else {
        if (iotConnStateTimer.autoTimeout(1000)) {
          mqttPublErrCnt.accu();
        }
      }
    }

    if (iotConnState == MQTT_STATE_FINISH_PREPARE_PUBMSG) {
      this->printlnFlush(mqttPublMsgPayload);
      mqttPublLock.release();
      mqttForcePublMode.off();
      iotConnState = MQTT_STATE_WAITING_PUBLISH;
    }

    if (iotConnState == MQTT_STATE_WAITING_PUBLISH) {
      this->listen();
      if (iotSerialRecv.indexOf(F("+QMTPUBEX: 0,0,0")) > -1
          || iotSerialRecv.indexOf(F("+QMTPUBEX: 0,1,0")) > -1
          || iotSerialRecv.indexOf(F("+QMTPUB: 0,0,0")) > -1
          || iotSerialRecv.indexOf(F("+QMTPUB: 0,1,0")) > -1) {
        this->clearRecv();
        iotConnState = MQTT_STATE_FINISH_PUBLISH;
      }
    }

    if (iotConnState == MQTT_STATE_FINISH_PUBLISH) {
      iotConnState = MQTT_STATE_INIT;  // finish one publish loop, loop-back
      iotSoftWatchdog.pet();
    }
  }

  void captureIMEI() {
    int idx = iotSerialRecv.indexOf(F("+CGSN: "));

    if (idx > -1) {
      {
        iotIMEI = iotSerialRecv.substring(idx + 8, idx + 8 + 15);
      }

      if (iotModel == IOT_MODEL_BC260Y_CN) {
        {
          iotIMEI = iotSerialRecv.substring(idx + 7, idx + 7 + 15);
        }
      }


      mqttConnCmd = F("AT+QMTCONN=0,dev_");
      mqttConnCmd.concat(iotIMEI);
      mqttConnCmd.concat(F(",tswh,1Wo=[6vA0m"));

      mqttSubsCmd = F("AT+QMTSUB=0,1,rgt/");
      mqttSubsCmd.concat(iotIMEI);
      mqttSubsCmd.concat(F("/out,0"));
    }
  }

  void captureCSQ() {
    if (iotSerialRecv.indexOf(F("+CSQ: ")) > -1) {
      int ws = iotSerialRecv.indexOf(F(": "));
      int we = iotSerialRecv.indexOf(F(","));

      {
        iotCSQ = iotSerialRecv.substring(ws + 2, we);
      }
    }
  }

  void captureCGATT() {
    if (iotSerialRecv.indexOf(F("+CGATT: 1")) > -1) {
      {
        iotCGATT = F("1");
      }
    }
  }

  void captureCEREG() {
    if (iotSerialRecv.indexOf(F("+CEREG: 0,1")) > -1) {
      iotCEREG = F("0,1");
    }
  }

  bool inspectCSQ() {
    byte CSQReading = 0;

    {
      CSQReading = iotCSQ.toInt();
    }

    return (CSQReading != 99 && CSQReading > 2);
  }

  bool inspectCGATT() {
    Serial.print(iotCGATT);
    return iotCGATT == F("1");
  }

  bool inspectCEREG() {
    return iotCEREG == F("0,1");
  }

  void queryParams() {
    if (iotConnState >= IOT_CONN_FINISH_INIT) {
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
  }


  void errHook() {
    if (iotSerialRecv.indexOf(F("+QIURC: \"pdpdeact\",1")) > -1) {
      iotModuleState = IOT_MODULE_WAITING_INIT;
    }

    if (iotSerialRecv.indexOf(F("+CME ERROR")) > -1) {
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

    if (mqttOpenErrCnt.over(20)) {
      Serial.print(F("mqttOpenErrCnt.over(20)"));
      iotModuleState = IOT_MODULE_WAITING_INIT;
    }

    if (mqttConnErrCnt.over(10)) {
      Serial.print(F("mqttConnErrCnt.over(10)"));
      iotModuleState = IOT_MODULE_WAITING_INIT;
    }

    if (mqttSubsErrCnt.over(10)) {
      Serial.print(F("mqttSubsErrCnt.over(10)"));
      iotModuleState = IOT_MODULE_WAITING_INIT;
    }

    if (mqttPublErrCnt.over(10)) {
      Serial.print(F("mqttPublErrCnt.over(10)"));
      iotModuleState = IOT_MODULE_WAITING_INIT;
    }
  }

public:
  void init() {
    // SerialIoT.begin(115200);
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

    this->manageModuleState();
    if (iotModuleState == IOT_MODULE_END_OF_STATE) {
      this->manageConnectionState();
    }

    {
      // this->manageMQTTState();
    }

    // this->handleMQTTSubs();
    // this->errHook();
  }

  void printlnFlush(const String& cmd) {
    SerialIoT.println(cmd);
    SerialIoT.flush();
    delay(1);
  }

  void buildMsg(byte _DIPayload, byte _DOPayload, const String& _AIPayload, const String& _AOPayload) {
    if (mqttPublLock.isReleased()) {
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
    }

    // 2. build prepare msg
    if (iotModel == IOT_MODEL_BC260Y_CN) {
      mqttPublMsgPrepare = F("AT+QMTPUB=0,1,2,0,rgt/");

    } else if (iotModel == IOT_MODEL_EC800K) {
      mqttPublMsgPrepare = F("AT+QMTPUBEX=0,1,2,0,rgt/");
    }
    mqttPublMsgPrepare.concat(iotIMEI);
    mqttPublMsgPrepare.concat(F("/in,"));
    mqttPublMsgPrepare.concat(mqttPublMsgPayload.length());
  }

  void forcePublish() {
    if (MQTT_STATE_INIT <= iotConnState && iotConnState <= MQTT_STATE_FINISH_PUBLISH) {
      if (!mqttForcePublMode.isOn()) {
        mqttForcePublMode.on();
      }
    }
  }

  void printExtractedRecv() {
    if (iotSerialRecv != F("")) {
      Serial.print(F("[["));
      Serial.print(iotSerialRecv);
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