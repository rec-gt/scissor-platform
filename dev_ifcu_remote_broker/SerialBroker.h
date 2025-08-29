#ifndef SerialBroker_H
#define SerialBroker_H

class SerialBroker {
private:
  String serialRecv = "";
  String serialSend = "";

  void clearSerialBuffer() {
    while (Serial1.read() > 0) { delay(1); };
  }

  void clearSerialRecv() {
    this->serialRecv = "";
  }

  void printlnFlush(String cmd, unsigned int delayTime = 2) {
    Serial1.println(cmd);
    Serial1.flush();
    delay(delayTime);
  }

  void capturePayload() {
    int idx = this->serialRecv.indexOf("PAYLOAD:");
    if (idx > -1) {
      this->serialRecvPayload = this->serialRecv.substring(8, 100);
    }
  }

public:
  String serialRecvPayload = "2500,2500,2,2,1700,3000";

  SerialBroker(){};

  void listen() {
    while (Serial1.available() > 0) {
      char c = Serial1.read();

      Serial.print(c);

      if (c != '\r' && c != '\n') {
        this->serialRecv += c;
      }

      if (c == '\r') {
        this->capturePayload();
        this->clearSerialRecv();
      }
    }
  }

  ~SerialBroker(){};
};

#endif