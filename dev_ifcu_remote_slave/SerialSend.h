#ifndef SerialSend_H
#define SerialSend_H


class SerialSend {
private:
  String serialSend = "";

  void clearSerialBuffer() {
    while (Serial2.read() > 0) { delay(1); };
  }

  void clearResBuffer() {
    this->serialSend = "";
  }

  void printlnFlush(String cmd, unsigned int delayTime = 2) {
    Serial2.println(cmd);
    Serial2.flush();
    delay(delayTime);
  }

  void capturePayload() {
    int idx = this->payload.indexOf("PAYLOAD:");
    if (idx > -1) {
      this->payload = this->payload.substring(8, 100);
    }
  }
public:
  String payload = "2500,2500,2,2,1700,3000";

  SerialSend(){};

  void listen() {
    while (Serial2.available() > 0) {
      char c = Serial2.read();

      Serial.print(c);

      if (c != '\r' && c != '\n') {
        this->serialSend += c;
      }

      if (c == '\r') {
        this->capturePayload();
        this->clearResBuffer();
      }
    }
  }

  ~SerialSend(){};
};

extern SerialSend serialSend;

#endif