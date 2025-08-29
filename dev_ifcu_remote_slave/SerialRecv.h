#ifndef SerialRecv_H
#define SerialRecv_H


class SerialRecv {
private:
  String serialRecv = "";

  void clearSerialBuffer() {
    while (Serial2.read() > 0) { delay(1); };
  }

  void clearResBuffer() {
    this->serialRecv = "";
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

  SerialRecv(){};

  void listen() {
    while (Serial2.available() > 0) {
      char c = Serial2.read();

      Serial.print(c);

      if (c != '\r' && c != '\n') {
        this->serialRecv += c;
      }

      if (c == '\r') {
        this->capturePayload();
        this->clearResBuffer();
      }
    }
  }

  ~SerialRecv(){};
};

extern SerialRecv serialRecv;

#endif