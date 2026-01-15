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
    this->parse();
    Serial.print(F("String Length: "));
    Serial.flush();
    Serial.println(iotSerialRecv.length());
    Serial.flush();
    Serial.println(iotSerialRecv);
    Serial.flush();
    this->clear();
  }

  void clear() {
    iotSerialRecv = F("");
  }

  void parse() {
    /* ====== Stateless Parameters ====== */
    /* === IMEI === */
    {
      iotParseIdx = iotSerialRecv.indexOf(F("+CGSN: "));
      if (iotParseIdx > -1) {
        if (iotModel == IOT_MODEL_EC800K) {
          iotIMEI = iotSerialRecv.substring(iotParseIdx + 8, iotParseIdx + 8 + 15);
        }

        if (iotModel == IOT_MODEL_BC260Y_CN) {
          iotIMEI = iotSerialRecv.substring(iotParseIdx + 7, iotParseIdx + 7 + 15);
        }

        mqttConnCmd = F("AT+QMTCONN=0,dev_");
        mqttConnCmd.concat(iotIMEI);
        mqttConnCmd.concat(F(",tswh,1Wo=[6vA0m"));

        mqttSubsCmd = F("AT+QMTSUB=0,1,rgt/");
        mqttSubsCmd.concat(iotIMEI);
        mqttSubsCmd.concat(F("/out,0"));
      }
    }

    /* === CSQ === */
    {
      iotParseIdx = iotSerialRecv.indexOf(F("+CSQ: "));
      if (iotParseIdx > -1) {
        iotCSQ = iotSerialRecv.substring(iotParseIdx + 6, iotParseIdx + 8);
        if (iotCSQ.length() > 1 && iotCSQ[1] == ',') {
          iotCSQ = iotCSQ.substring(0, 1);
        }
      }
    }

    /* === CGATT === */
    {
      if (iotSerialRecv.indexOf(F("+CGATT: 1")) > -1) {
        iotCGATT = F("1");
      }
    }

    /* === CEREG === */
    {
      if (iotSerialRecv.indexOf(F("+CEREG: 0,1")) > -1) {
        iotCEREG = F("0,1");
      }
    }

    /* === Subscribed Message === */
    {
      iotParseIdx = iotSerialRecv.indexOf(F("+QMTRECV: "));
      if (iotParseIdx > -1) {
        mqttSubsMsgContent = iotSerialRecv.substring(iotParseIdx + 41, iotParseIdx + 46);
        Serial.println(mqttSubsMsgContent);
      }
    }

    /* === Error Hook === */
    {
      this->errHook();
    }

    /* ====== Parameters with state ====== */
    {
      this->manageModuleState();
      if (iotModuleState == IOT_MODULE_END_OF_STATE) {
        this->manageConnectionState();
        if (iotConnState == IOT_CONN_END_OF_STATE) {
          this->manageMqttMessageState();
        }
      }
    }
  }

  void manageModuleState() {
    if (iotModuleState == IOT_MODULE_WAITING_INIT) {
      Serial.println(F(">>> IOT MODULE INIT, RESET"));
      iotModuleState = IOT_MODULE_WAITING_RESET;
      iotConnState = IOT_CONN_WAITING_INIT;
      iotMqttMsgState = IOT_MQTT_MSG_LOOP_START;

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
      if (iotSerialRecv.indexOf(F("RDY")) > -1) {
        Serial.println(F(">>> FINISH HARDWARE RESET"));
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
      iotModuleState = IOT_MODULE_FINISH_RESET;
    }

    if (iotModuleState == IOT_MODULE_FINISH_RESET) {
      if (iotModuleStateTimer.autoTimeout(1000)) {
        this->printlnFlush(F("ATI"));
        iotModuleState = IOT_MODULE_WAITING_GET_MODEL;
      }
    }

    if (iotModuleState == IOT_MODULE_WAITING_GET_MODEL) {
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
      if (iotIMEI.length() == 15) {
        iotConnState = IOT_CONN_WAITING_CSQ;
      }
    }

    if (iotConnState == IOT_CONN_WAITING_CSQ) {
      if (iotConnStateTimer.autoTimeout(1000)) {
        this->printlnFlush(F("AT+CSQ"));
      }
      if (this->inspectCSQ()) {
        iotConnState = IOT_CONN_FINISH_CSQ;
        iotSoftWatchdog.pet();
      }
    }

    if (iotConnState == IOT_CONN_FINISH_CSQ) {
      iotConnState = IOT_CONN_WAITING_CGATT;
    }

    if (iotConnState == IOT_CONN_WAITING_CGATT) {
      if (iotConnStateTimer.autoTimeout(1000)) {
        this->printlnFlush(F("AT+CGATT?"));
      }
      if (this->inspectCGATT()) {
        iotConnState = IOT_CONN_FINISH_CGATT;
        iotSoftWatchdog.pet();
      }
    }

    if (iotConnState == IOT_CONN_FINISH_CGATT) {
      iotConnState = IOT_CONN_WAITING_CEREG;
    }

    if (iotConnState == IOT_CONN_WAITING_CEREG) {
      if (iotConnStateTimer.autoTimeout(1000)) {
        this->printlnFlush(F("AT+CEREG?"));
      }
      if (this->inspectCEREG()) {
        iotConnState = IOT_CONN_FINISH_CEREG;
        iotSoftWatchdog.pet();
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
      if (iotSerialRecv.indexOf(F("+QMTOPEN: 0,0")) > -1) {
        iotConnState = IOT_CONN_FINISH_OPEN_MQTT;
        iotSoftWatchdog.pet();
      }
    }

    if (iotConnState == IOT_CONN_FINISH_OPEN_MQTT) {
      if (iotConnStateTimer.autoTimeout(1000)) {
        this->printlnFlush(mqttConnCmd);
        iotConnState = IOT_CONN_WAITING_CONN_MQTT;
      }
    }

    if (iotConnState == IOT_CONN_WAITING_CONN_MQTT) {
      if (iotSerialRecv.indexOf(F("+QMTCONN: 0,0,0")) > -1) {
        iotConnState = IOT_CONN_FINISH_CONN_MQTT;
        iotSoftWatchdog.pet();
      }
    }

    if (iotConnState == IOT_CONN_FINISH_CONN_MQTT) {
      if (iotConnStateTimer.autoTimeout(1000)) {
        this->printlnFlush(mqttSubsCmd);
        iotConnState = IOT_CONN_WAITING_SUBS_MQTT_TOPIC;
      }
    }

    if (iotConnState == IOT_CONN_WAITING_SUBS_MQTT_TOPIC) {
      if (iotSerialRecv.indexOf(F("+QMTSUB: 0,1,0,0")) > -1) {
        iotConnState = IOT_CONN_FINISH_SUBS_MQTT_TOPIC;
        iotConnState = IOT_CONN_FINISH_INIT;
        iotSoftWatchdog.pet();
      }
    }

    if (iotConnState == IOT_CONN_FINISH_INIT) {
      iotMqttMsgState = IOT_MQTT_MSG_LOOP_START;
      iotConnState = IOT_CONN_END_OF_STATE;
    }
  }

  void manageMqttMessageState() {
    if (iotMqttMsgState == IOT_MQTT_MSG_LOOP_START) {
      this->queryParams();

      if (mqttForcePublMode.isOn()) {
        mqttForcePublMode.off();
        mqttPublLock.lock();
        this->printlnFlush(mqttPublMsgPrepare);
        iotMqttMsgState = IOT_MQTT_MSG_WAITING_PUBLISH;
      } else {
        if (iotConnStateTimer.autoTimeout(10000)) {
          mqttPublLock.lock();
          this->printlnFlush(mqttPublMsgPrepare);
          iotMqttMsgState = IOT_MQTT_MSG_WAITING_PUBLISH;
        }
      }
    }

    if (iotSerialRecv.indexOf(F(">")) > -1) {
      if (iotMqttMsgState == IOT_MQTT_MSG_WAITING_PUBLISH) {
        this->printlnFlush(mqttPublMsgPayload);
        mqttPublLock.release();
        iotMqttMsgState = IOT_MQTT_MSG_WAITING_PUBLISH_ACK;
      }
    }

    if (iotSerialRecv.indexOf(F("+QMTPUBEX: 0,0,0")) > -1 || iotSerialRecv.indexOf(F("+QMTPUBEX: 0,1,0")) > -1
        || iotSerialRecv.indexOf(F("+QMTPUB: 0,0,0")) > -1 || iotSerialRecv.indexOf(F("+QMTPUB: 0,1,0")) > -1) {
      if (iotMqttMsgState == IOT_MQTT_MSG_WAITING_PUBLISH_ACK) {
        iotSoftWatchdog.pet();
        iotMqttMsgState = IOT_MQTT_MSG_FINISH_PUBLISH;
        iotMqttMsgState = IOT_MQTT_MSG_LOOP_START;  // finish one publish, loop-back
      }
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
    return iotCGATT == F("1");
  }

  bool inspectCEREG() {
    return iotCEREG == F("0,1");
  }

  void queryParams() {
    if (mqttPublLock.isReleased()) {
      if (iotParamTimer.autoTimeout(3000)) {
        Serial.println(F(">>> Query?"));
        this->printlnFlush(F("AT+CPIN?"));
        this->printlnFlush(F("AT+CSQ"));
        this->printlnFlush(F("AT+CGATT?"));
        this->printlnFlush(F("AT+CEREG?"));
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
    this->listen();
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
    if (!mqttForcePublMode.isOn()) {
      Serial.println(F("Force Publish:"));
      mqttForcePublMode.on();
    }
  }
};


extern IoT iot;

#endif

// A  T  +  C  S  Q  +  C  S  Q  :  _  2  4  ,  9  9  O  K
// 41 54 2B 43 53 51 2B 43 53 51 3A 20 32 34 2C 39 39 4F 4B 0