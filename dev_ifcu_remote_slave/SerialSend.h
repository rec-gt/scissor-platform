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

public:
  String payload = "2500,2,2";

  SerialSend(){};

  void send() {
    this->printlnFlush(payload);
  }

  ~SerialSend(){};
};

extern SerialSend serialSend;

#endif