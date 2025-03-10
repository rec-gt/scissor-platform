#define NBIoTModule Serial1

class NBIoT {
private:

  enum NBIOTStatus {
    // allow when NBIOT_AVAILABLE = false
    IOT_VOID,
    IOT_INIT_CLAC,
    IOT_INIT_9600,
    IOT_INIT_AT,
    IOT_INIT_CSQ,
    IOT_INIT_CEREG,
    IOT_INIT_CGATT,
    IOT_INIT_CIMI,
    IOT_INIT_CGSN,
    IOT_INIT_MQTTDISC,
    IOT_INIT_MQTTDEL,
    IOT_INIT_MQTTCFG,
    IOT_INIT_MQTTOPEN,

    // allow when NBIOT_AVAILABLE = true
    IOT_SYS_RUNNING,
    IOT_SYS_STOPPED,
    IOT_SYS_LIFTUP,
    IOT_SYS_FAILURE
  };

  NBIOTStatus NBIOT_STATUS = IOT_VOID;
  NBIOTStatus LAST_NBIOT_STATUS = IOT_VOID;
  bool NBIOT_AVAILABLE = false;

  unsigned long deadline;

  bool isSentCMD = false;
  bool isReceived = false;

  byte errCount = 0;
  byte RN = 2;
  String CIMI;
  String CSQ;
  String IMEI;
  String response;

  void clearBuffer() {
    while (NBIoTModule.read() >= 0) {}
  }

  void resetErrorCount() {
    this->errCount = 0;
  }

  void resetModule() {
    // cut power
    // connect after 5s
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

    if (this->errCount >= 10) {
      Serial.println("MQTT init failed");
      this->resetModule();
    }
  }

public:
  NBIoT() {
    NBIoTModule.begin(9600);
  };

  void setStatus() {
    this->LAST_NBIOT_STATUS = this->NBIOT_STATUS = IOT_VOID;
  }

  void resetSendReceive() {
    this->isSentCMD = false;
    this->isReceived = false;
  }

  void setDeadline(int time) {
    this->deadline = millis() + time;
  }

  void sendCommand(String cmd) {
    if (!this->isSentCMD) {  // ensure send once
      this->isSentCMD = true;
      NBIoTModule.println(cmd);
      delay(300);
    }

    if (millis() > this->deadline) {  // run in main loop, check for timesup
      this->NBIOT_STATUS = IOT_VOID;
    }
  }

  void waitForResponse() {  // run in main loop, always wait for response
    if (NBIoTModule.available()) {
      this->isReceived = false;
      this->response = NBIoTModule.readString();
      Serial.println(this->response);
      this->clearBuffer();
    }
  }

  void handleAT() {
    if (this->NBIOT_STATUS == IOT_INIT_AT) {
      if (this->LAST_NBIOT_STATUS != this->NBIOT_STATUS) {  // delta change is detected
        Serial.println("Send AT");
        this->LAST_NBIOT_STATUS = this->NBIOT_STATUS;
        this->resetSendReceive();
        this->setDeadline(9000);
      } else {  // delta change is not detected, still in main loop
        this->sendCommand("AT");
        this->waitForResponse();
        // business logic & change status
        if (this->isReceived) {
          if (this->resContain("OK") && !this->resContain("ERROR")) {
            this->NBIOT_STATUS = IOT_INIT_CIMI;
          } else {
            this->NBIOT_STATUS = IOT_VOID;
          }
        }
      }
    }
  }

  void handleCIMI() {
    if (this->NBIOT_STATUS == IOT_INIT_CIMI) {
      if (this->LAST_NBIOT_STATUS != this->NBIOT_STATUS) {  // delta change is detected
        Serial.println("Send CIMI");
        this->LAST_NBIOT_STATUS = this->NBIOT_STATUS;
        this->resetSendReceive();
        this->setDeadline(3000);
      } else {  // delta change is not detected, still in main loop
        this->sendCommand("CIMI");
        this->waitForResponse();
        // business logic & change status
        if (this->resContain("OK") && !this->resContain("ERROR")) {
          this->parseCIMI();
          this->NBIOT_STATUS = IOT_VOID;
        }
      }
    }
  }

  void listen() {
    if (this->NBIOT_STATUS == IOT_VOID) {
      this->NBIOT_STATUS = IOT_INIT_AT;
    }

    this->handleAT();
    this->handleCIMI();
  }

  ~NBIoT(){};
};
