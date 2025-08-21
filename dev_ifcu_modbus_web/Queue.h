#ifndef Queue_h
#define Queue_h

class Queue {

private:
  byte que[5];
  int idx = -1;
public:
  int length = 0;

  Queue(){};

  void push(byte data) {
    if (idx >= 4) {
      return;
    }

    que[++idx] = data;
    length++;

    debug();
  }

  byte pop() {
    if (idx <= -1) {
      return;
    }

    byte res = que[0];

    for (byte i = 1; i < 5; i++) {
      que[i - 1] = que[i];
    }
    que[4] = 0;

    idx--;
    length--;
    debug();
    return res;
  }

  void debug() {
    Serial.println();

    for (byte i = 0; i < 5; i++) {
      Serial.print(que[i]);
      Serial.print(", ");
    }
    Serial.println("idx: " + idx);
    Serial.println("len: " + length);
  }

  ~Queue(){};
};

extern Queue que;

#endif