#ifndef Queue_h
#define Queue_h

class Queue {

private:
  byte que[5];
  byte idx = -1;
public:
  byte length = 0;

  Queue(){};

  void push(byte data) {
    if (idx >= 4) {
      return;
    }

    que[idx++] = data;
    length++;
  }

  byte pop() {
    if (idx <= 0) {
      return;
    }

    byte res = que[0];

    for (byte i = 1; i < 5; i++) {
      que[i - 1] = que[i];
    }

    idx--;
    length--;

    return res;
  }

  void debug() {
    for (byte i = 0; i < 5; i++) {
      Serial.print(que[i]);
      Serial.print(", ");
    }
    Serial.println(idx);
    Serial.println(length);
  }

  ~Queue(){};
};

extern Queue que;

#endif