#ifndef AsyncSerial_h
#define AsyncSerial_h

#define NBIoT_Module Serial1

class AsyncSerial {
private:
  String res = "";

  void pruneResBuffer() {
    this->res = "";
  }
public:
  AsyncSerial() {}

  void waitMsg() {
    if (NBIoT_Module.available() > 0) {
      char byte = NBIoT_Module.read();
      if (byte != '\r' && byte != '\n') {
        this->res += byte;
      }

      if (byte == '\r') {
        handleATCommand(res);
        pruneResBuffer();
      }
    }
  }


  ~AsyncSerial() {}
};