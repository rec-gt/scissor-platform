#ifndef Queue_h
#define Queue_h

class Queue {

private:
  byte queue[5];
  byte idx = -1;
public:
  byte length = 0;

  Queue(){};

  void push(byte data) {
    if (idx >= 9) {
      return;
    }

    queue[idx++] = data;
    length++;
  }

  byte pop() {
    if (idx <= 0) {
      return;
    }

    byte res = queue[0];

    for (byte i = 1; i < 5; i++) {
      queue[i - 1] = queue[i];
    }

    idx--;
    length--;

    return res;
  }

  ~Queue();
};

extern Queue queue;

#end