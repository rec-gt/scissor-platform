#include "./AsyncTimer.h"
#include "./Utils.h"
#include "./Watchdog.h"
#include "./Globals.h"

#ifndef IoT_h
#define IoT_h

#define IOT_MODULE_RESET_PIN 24

class IoTTimer {
private:
  byte state = 0;  // 0, 1
  uint16_t prevMillis = 0;

public:
  bool delay(uint16_t timeout) {
    // auto init timer, must happen once
    if (this->state == 0) {
      this->prevMillis = millis();  // renew the time
      this->state = 1;
    }

    // final decision
    bool isTimeout = (millis() - this->prevMillis) > timeout;

    if (isTimeout) {
      this->state = 0;
    }

    return isTimeout;
  }
};

IoTTimer iotTimer;

Watchdog iotWatchdog(3000UL);

class IoT {
private:
  enum IOT_STATE {
    IOT_STATE_WAITING_INIT,

    IOT_STATE_WAITING_RESET,
    IOT_STATE_FINISH_RESET,
    IOT_STATE_WAITING_RDY,
    IOT_STATE_FINISH_RDY,
    IOT_STATE_WAITING_IP,
    IOT_STATE_FINISH_IP,
    IOT_STATE_WAITING_CONFIG,
    IOT_STATE_FINISH_CONFIG,
    IOT_STATE_WAITING_CSQ,
    IOT_STATE_FINISH_CSQ,
    IOT_STATE_WAITING_CGATT,
    IOT_STATE_FINISH_CGATT,
    IOT_STATE_WAITING_CEREG,
    IOT_STATE_FINISH_CEREG,
    IOT_STATE_WAITING_OPEN,
    IOT_STATE_FINISH_OPEN,
    IOT_STATE_WAITING_CONN,
    IOT_STATE_FINISH_CONN,
    IOT_STATE_WAITING_SUB,
    IOT_STATE_FINISH_SUB,

    IOT_STATE_FINISH_INIT,
  };

  enum PUBSUB_PIPELINE {
    PIPELINE_DEFAULT,
    PIPELINE_WAITING_CSQ,
    PIPELINE_FINISH_CSQ,
    PIPELINE_WAITING_CGATT,
    PIPELINE_FINISH_CGATT,
    PIPELINE_WAITING_CEREG,
    PIPELINE_FINISH_CEREG,
    PIPELINE_WAITING_PREPARE_PUBMSG,
    PIPELINE_FINISH_PREPARE_PUBMSG,
    PIPELINE_WAITING_PUBLISH,
    PIPELINE_FINISH_PUBLISH,
  };

  void clearSerialBuffer() {
    while (SerialIoT.read() > 0) { delay(1); };
  }

  void clearRecvBuffer() {
    iotSerialRecv = F("");
  }

  void resetConfigs() {
    iotSerialRecv = F("");

    iotCSQ = F("");
    iotIMEI = F("");
    iotCGATT = F("");
    iotCEREG = F("");

    mqttConnCmd = F("");
    mqttSubsCmd = F("");

    mqttPublMsgPayloadLock = false;
    mqttPublMsgPrepare = F("");
    mqttPublMsgPayload = F("");
    mqttPublMsgCommand = F("");
    mqttSubsMsgContent = F("");

    digitalWrite(IOT_MODULE_RESET_PIN, LOW);
    if (iotTimer.delay(1000)) {
      digitalWrite(IOT_MODULE_RESET_PIN, HIGH);
      Serial.println(F("\r\n>>> MODULE FINISH POWER RESET"));
      this->connState = IOT_STATE_FINISH_RESET;
    }
  }

  void resetBuffers() {
    this->clearSerialBuffer();
    this->clearRecvBuffer();
    delay(1);
  }

  void resetModule() {
    this->resetConfigs();
    this->resetBuffers();
  }

  void printlnFlush(const String& cmd, unsigned int delayTime = 2) {
    SerialIoT.println(cmd);
    SerialIoT.flush();
    delay(delayTime);
  }

  void monitorReset() {
    if (this->connState == IOT_STATE_WAITING_INIT || this->connState == IOT_STATE_WAITING_RESET) {
      this->resetModule();
    }
  }

  void query() {
    if (this->connState == IOT_STATE_FINISH_RESET) {
      this->connState = IOT_STATE_WAITING_RDY;
    }

    if (this->connState == IOT_STATE_FINISH_RDY) {
      if (iotTimer.delay(500)) {
        this->printlnFlush(F("AT+CGSN=1"));
        this->printlnFlush(F("AT+QSCLK=0"));
        this->printlnFlush(F("AT+QIDNSCFG=0,223.5.5.5,8.8.8.8"));
        this->connState = IOT_STATE_WAITING_IP;
      }
    }

    if (this->connState == IOT_STATE_FINISH_IP) {
      Serial.println(F("\r\n>>> SETTING UP IOT"));
      this->printlnFlush(F("AT+CFUN=1"));
      this->printlnFlush(F("AT+QSCLK=0"));

      if (false) {  // TODO: only applicable to NB-IoT
        this->printlnFlush(F("AT+CPSMS=0"));
        this->printlnFlush(F("AT+CSCON=0"));
        this->printlnFlush(F("AT+CEDRXS=0,5"));
      }

      this->printlnFlush(F("AT+QMTCLOSE=0"));
      this->printlnFlush(F("AT+QMTDISC=0"));
      this->connState = IOT_STATE_WAITING_CONFIG;
    }

    if (this->connState == IOT_STATE_FINISH_CONFIG) {
      if (iotTimer.delay(1000UL)) {
        Serial.println(F("\r\n>>> GETTING CSQ"));

        this->printlnFlush(F("AT+CSQ"));

        this->connState = IOT_STATE_WAITING_CSQ;
      }
    }

    if (this->connState == IOT_STATE_FINISH_CSQ) {
      if (iotTimer.delay(1000UL)) {
        Serial.println(F("\r\n>>> GETTING CGATT"));

        this->printlnFlush(F("AT+CGATT?"));

        this->connState = IOT_STATE_WAITING_CGATT;
      }
    }

    if (this->connState == IOT_STATE_FINISH_CGATT) {
      if (iotTimer.delay(1000UL)) {
        Serial.println(F("\r\n>>> GETTING CEREG"));

        this->printlnFlush(F("AT+CEREG?"));

        this->connState = IOT_STATE_WAITING_CEREG;
      }
    }

    if (this->connState == IOT_STATE_FINISH_CEREG) {
      if (iotTimer.delay(1000UL)) {
        Serial.println(F("\r\n>>> OPENING MQTT..."));
        // this->printlnFlush(F("AT+QMTOPEN=0,8.210.84.24,1880"));
        this->printlnFlush(F("AT+QMTOPEN=0,iot.rec-gt.com,1880"));

        this->connState = IOT_STATE_WAITING_OPEN;
      }
    }

    if (this->connState == IOT_STATE_FINISH_OPEN) {
      if (iotTimer.delay(1000UL)) {
        Serial.println(F("\r\n>>> CONNECTING MQTT..."));

        this->printlnFlush(mqttConnCmd);
        this->connState = IOT_STATE_WAITING_CONN;
      }
    }

    if (this->connState == IOT_STATE_FINISH_CONN) {
      Serial.println(F("\r\n>>> SUBSCRBING TOPIC..."));

      this->printlnFlush(mqttSubsCmd);
      this->connState = IOT_STATE_WAITING_SUB;
    }

    if (this->connState == IOT_STATE_FINISH_SUB) {
      // this->finishInit = true;
      this->connState = IOT_STATE_FINISH_INIT;
      Serial.println(F("\r\n>>> FINISH INIT IOT"));
    }

    if (this->connState == IOT_STATE_FINISH_INIT) {
      if (this->pipelineState == PIPELINE_DEFAULT) {
        if (iotTimer.delay(5000)) {
          Serial.println(F("\r\n>>> [CONNECTED] QUERYING CSQ"));
          this->printlnFlush(F("AT+CSQ"));

          this->pipelineState = PIPELINE_WAITING_CSQ;
        }
      }

      if (this->pipelineState == PIPELINE_FINISH_CSQ) {
        if (iotTimer.delay(5000)) {
          Serial.println(F("\r\n>>> [CONNECTED] QUERYING CGATT"));
          this->printlnFlush(F("AT+CGATT?"));

          this->pipelineState = PIPELINE_WAITING_CGATT;
        }
      }

      if (this->pipelineState == PIPELINE_FINISH_CGATT) {
        if (iotTimer.delay(5000)) {
          Serial.println(F("\r\n>>> [CONNECTED] QUERYING CEREG"));
          this->printlnFlush(F("AT+CEREG?"));

          this->pipelineState = PIPELINE_WAITING_CEREG;
        }
      }

      if (this->pipelineState == PIPELINE_FINISH_CEREG) {
        if (iotTimer.delay(13000)) {
          Serial.println(F("\r\n>>> EXECUTE REGULAR PUBLISH"));

          this->printlnFlush(mqttPublMsgPrepare);
          mqttPublMsgPayloadLock = true;  // disable the preparation of payload
          this->pipelineState = PIPELINE_WAITING_PREPARE_PUBMSG;
        }
      }

      if (this->pipelineState == PIPELINE_FINISH_PREPARE_PUBMSG) {
        if (iotTimer.delay(2000)) {
          this->printlnFlush(mqttPublMsgPayload);
          mqttPublMsgPayloadLock = false;  // release the lock
          this->pipelineState = PIPELINE_WAITING_PUBLISH;
        }
      }
    }
  }

  void listen() {
    while (SerialIoT.available() > 0) {
      char c = SerialIoT.read();

      if (iotDebugMode) {
        Serial.print(c);
      }

      if (c != '\r' && c != '\n') {
        iotSerialRecv += c;
      }

      if (c == '\r') {
        this->answer();
        this->extractData();
        this->clearRecvBuffer();
      }
    }

    if (iotSoftReset) {
      iotSoftReset = false;
      Serial.println(F("[SOFT_RESET]"));
      this->connState = IOT_STATE_WAITING_RESET;
    }
  }

  void answer() {
    int idx = -1;

    if (this->connState == IOT_STATE_WAITING_RDY) {
      iotCmpStr = F("RDY");
      idx = iotSerialRecv.indexOf(iotCmpStr);
      if (idx > -1) {
        Serial.println(F("\r\n>>> IOT READY"));
        this->connState = IOT_STATE_FINISH_RDY;
        iotWatchdog.pet();
      }
    }

    if (this->connState == IOT_STATE_WAITING_IP) {
      if (true) {  // TODO: sudo 4g
        this->connState = IOT_STATE_FINISH_IP;
      } else {
        iotCmpStr = F("+IP:");
        idx = iotSerialRecv.indexOf(iotCmpStr);
        if (idx > -1) {
          Serial.println(F("\r\n>>> FINISH WAITING IP"));

          this->connState = IOT_STATE_FINISH_IP;
          iotWatchdog.pet();
        } else {
          if (iotTimer.delay(1000)) {
            this->connState = IOT_STATE_FINISH_IP;  // by-pass
          }
        }
      }
    }

    if (this->connState == IOT_STATE_WAITING_CONFIG) {
      {
        Serial.println(F("\r\n>>> FINISH SETUP"));

        this->connState = IOT_STATE_FINISH_CONFIG;
        iotWatchdog.pet();
      }
    }

    if (this->connState == IOT_STATE_WAITING_CSQ) {
      iotCmpStr = F("+CSQ:");
      idx = iotSerialRecv.indexOf(iotCmpStr);
      if (idx > -1) {
        Serial.println(F("\r\n>>> FINISH GETTING CSQ"));

        this->connState = IOT_STATE_FINISH_CSQ;
        iotWatchdog.pet();
      }
    }

    if (this->connState == IOT_STATE_WAITING_CGATT) {
      iotCmpStr = F("+CGATT:");
      idx = iotSerialRecv.indexOf(iotCmpStr);
      if (idx > -1) {
        Serial.println(F("\r\n>>> FINISH GETTING CGATT"));

        this->connState = IOT_STATE_FINISH_CGATT;
        iotWatchdog.pet();
      }
    }

    if (this->connState == IOT_STATE_WAITING_CEREG) {
      iotCmpStr = F("+CEREG:");
      idx = iotSerialRecv.indexOf(iotCmpStr);
      if (idx > -1) {
        Serial.println(F("\r\n>>> FINISH GETTING CEREG"));

        this->connState = IOT_STATE_FINISH_CEREG;
        iotWatchdog.pet();
      }
    }

    if (this->connState == IOT_STATE_WAITING_OPEN) {
      iotCmpStr = F("+QMTOPEN: 0,0");
      idx = iotSerialRecv.indexOf(iotCmpStr);
      if (idx > -1) {
        Serial.println(F("\r\n>>> OPENED MQTT"));

        this->connState = IOT_STATE_FINISH_OPEN;
        iotWatchdog.pet();
      }
    }

    if (this->connState == IOT_STATE_WAITING_CONN) {
      iotCmpStr = F("+QMTCONN: 0,0,0");
      idx = iotSerialRecv.indexOf(iotCmpStr);
      if (idx > -1) {
        Serial.println(F("\r\n>>> CONNECTED MQTT"));

        this->connState = IOT_STATE_FINISH_CONN;
        iotWatchdog.pet();
      }
    }

    if (this->connState == IOT_STATE_WAITING_SUB) {
      iotCmpStr = F("+QMTSUB:");
      idx = iotSerialRecv.indexOf(iotCmpStr);

      if (idx > -1) {
        Serial.println(F("\r\n>>> SUB TOPIC OK"));

        this->connState = IOT_STATE_FINISH_SUB;
        iotWatchdog.pet();
      }
    }

    if (this->connState == IOT_STATE_FINISH_INIT) {
      int idx = -1;
      if (this->pipelineState == PIPELINE_WAITING_CSQ) {

        iotCmpStr = F("+CSQ:");
        idx = iotSerialRecv.indexOf(iotCmpStr);

        if (idx > -1) {
          Serial.println(F("\r\n>>> FINISH GETTING CSQ"));

          this->pipelineState = PIPELINE_FINISH_CSQ;
          iotWatchdog.pet();
        }
      }

      if (this->pipelineState == PIPELINE_WAITING_CGATT) {

        iotCmpStr = F("+CGATT:");
        idx = iotSerialRecv.indexOf(iotCmpStr);

        if (idx > -1) {
          Serial.println(F("\r\n>>> FINISH GETTING CGATT"));

          this->pipelineState = PIPELINE_FINISH_CGATT;
          iotWatchdog.pet();
        }
      }

      if (this->pipelineState == PIPELINE_WAITING_CEREG) {

        iotCmpStr = F("+CEREG:");
        idx = iotSerialRecv.indexOf(iotCmpStr);

        if (idx > -1) {
          Serial.println(F("\r\n>>> FINISH GETTING CEREG"));

          this->pipelineState = PIPELINE_FINISH_CEREG;
          iotWatchdog.pet();
        }
      }

      if (this->pipelineState == PIPELINE_WAITING_PREPARE_PUBMSG) {

        iotCmpStr = F(">");
        idx = iotSerialRecv.indexOf(iotCmpStr);

        if (idx > -1) {
          this->pipelineState = PIPELINE_FINISH_PREPARE_PUBMSG;
          iotWatchdog.pet();
        }
      }

      if (this->pipelineState == PIPELINE_WAITING_PUBLISH) {

        iotCmpStr = F("+QMTPUB: 0,0,0");
        idx = iotSerialRecv.indexOf(iotCmpStr);

        if (idx > -1) {
          this->pipelineState = PIPELINE_DEFAULT;
          Serial.println(F("\r\n>>> FINISH REGULAR PUBLISH"));

          iotWatchdog.pet();
        }
      }
    }
  }

  void extractData() {
    int idx = -1;

    // === handle IMEI ===

    iotCmpStr = F("+CGSN:");
    idx = iotSerialRecv.indexOf(iotCmpStr);

    if (idx > -1) {
      {
        iotIMEI = iotSerialRecv.substring(8, 8 + 15);
        Serial.print(iotIMEI);

        if (!utils.isNumeric(iotIMEI)) {
          Serial.print(F("Not Numeric"));
          iotSoftReset = true;
        }

        if (iotIMEI.length() != 15) {
          Serial.print(F("Not Numeric"));
          iotSoftReset = true;
        }

        mqttConnCmd = F("AT+QMTCONN=0,dev_");
        mqttConnCmd.concat(iotIMEI);
        mqttConnCmd.concat(F(",tswh,1Wo=[6vA0m"));

        mqttSubsCmd = F("AT+QMTSUB=0,1,rgt/");
        mqttSubsCmd.concat(iotIMEI);
        mqttSubsCmd.concat(F("/out,0"));
      }
    }

    // === handle CGATT ===
    iotCmpStr = F("+CGATT:");
    idx = iotSerialRecv.indexOf(iotCmpStr);

    if (idx > -1) {
      {
        iotCGATT = iotSerialRecv.substring(8, 8 + 1);
      }

      if (iotCGATT != F("1")) {
        iotSoftReset = true;
      }
    }

    // === handle CEREG ===

    iotCmpStr = F("+CEREG:");
    idx = iotSerialRecv.indexOf(iotCmpStr);

    if (idx > -1) {
      {
        iotCEREG = iotSerialRecv.substring(8, 8 + 3);
      }

      if (iotCEREG != F("0,1")) {
        iotSoftReset = true;
      }
    }

    // === handle CSQ ===

    iotCmpStr = F("+CSQ:");
    idx = iotSerialRecv.indexOf(iotCmpStr);

    if (idx > -1) {
      iotCmpStr = F(": ");
      int winStart = iotSerialRecv.indexOf(iotCmpStr);
      iotCmpStr = F(",");
      int winEnd = iotSerialRecv.indexOf(iotCmpStr);

      {
        iotCSQ = iotSerialRecv.substring(winStart + 2, winEnd);
      }

      if (iotCSQ == F("99")) {
        iotSoftReset = true;
      }

      if (!utils.isNumeric(iotCSQ)) {
        iotSoftReset = true;
        iotCSQ = F("0");
      }

      {
        int numCSQ = iotCSQ.toInt();
        if (!(numCSQ >= 5 && numCSQ <= 31)) {
          iotSoftReset = true;
          iotCSQ = F("0");
        }
      }
    }

    // === handle publish ACK ===
    iotCmpStr = F("+QMTPUB:");
    idx = iotSerialRecv.indexOf(iotCmpStr);

    if (idx > -1) {

      {
        mqttPubAck = iotSerialRecv.substring(9, 9 + 5);
      }

      if (mqttPubAck != F("0,0,0")) {
        iotSoftReset = true;
      }
    }

    // === handle SUB ACK ===
    iotCmpStr = F("+QMTSUB:");
    idx = iotSerialRecv.indexOf(iotCmpStr);

    if (idx > -1) {

      {
        mqttSubAck = iotSerialRecv.substring(9, 9 + 7);
      }

      if (mqttSubAck != F("0,1,0,0")) {
        iotSoftReset = true;
      }
    }

    // === handle SUB received msg and parse it's content ===
    iotCmpStr = F("+QMTRECV:");
    idx = iotSerialRecv.indexOf(iotCmpStr);

    if (idx > -1) {
      {
        mqttSubsMsgContent = iotSerialRecv.substring(41, 46);
        Serial.println(mqttSubsMsgContent);
      }
    }
  }

public:
  IOT_STATE connState = IOT_STATE_WAITING_INIT;
  PUBSUB_PIPELINE pipelineState = PIPELINE_DEFAULT;

  IoT() {
    pinMode(IOT_MODULE_RESET_PIN, OUTPUT);
    digitalWrite(IOT_MODULE_RESET_PIN, HIGH);
  }

  void debug() {
    iotDebugMode = true;
  }

  void init() {
    Serial.println(F("\r\n>>> === IOT START ==="));
    SerialIoT.begin(115200);

    iotWatchdog.enable();
    iotWatchdog.setCallback([]() {
      iotSoftReset = true;
    });
  }

  void loop() {
    this->monitorReset();
    this->query();
    this->listen();
    iotConnState = this->connState;  // update global variable for display
  }

  void forcePublish() {
    if (this->connState == IOT_STATE_FINISH_INIT) {
      if (this->pipelineState == PIPELINE_WAITING_PREPARE_PUBMSG
          || this->pipelineState == PIPELINE_FINISH_PREPARE_PUBMSG
          || this->pipelineState == PIPELINE_WAITING_PUBLISH
          || this->pipelineState == PIPELINE_FINISH_PUBLISH) {
        return;
      } else {
        Serial.println(F("\r\n>>> FORCE PUBLISH STOPPED"));

        this->printlnFlush(mqttPublMsgCommand);
      }
    }
  }

  ~IoT() {}
};

extern IoT iot;

#endif