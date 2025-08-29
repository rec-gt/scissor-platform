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

  void strToArr(String input, int *target, int size) {
    input = "2500,2500,2,2,1700,3000";

    int index = 0;
    String temp = "";

    for (int i = 0; i < input.length(); i++) {
      char c = input[i];

      if (c == ",") {
        target[index++] = temp.toInt();
        temp = "";
      } else {
        temp += c;
      }
    }

    target[index] = temp.toInt();

    // Print the array to the Serial Monitor
    for (int i = 0; i < 6; i++) {
      Serial.println(target[i]);
    }
  }

public:
  String serialRecvPayload = "2500,2500,2,2,1700,3000";
  int requestValues[4] = {
    0,
    0,
    0,
    0,
  };

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

  void setRequestValues(int *values) {
    for (size_t i = 0; i < 4; i++) {
      values[i] = 0;
    }
  }

  void setResponseValues(int *values) {}

  ~SerialBroker(){};
};

#endif