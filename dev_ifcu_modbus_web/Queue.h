class Queue {

private:
  byte queue[10];
  byte index = 0;

public:
  Queue(size_t size)
    : size(size){};

  void push(byte data) {
    if (index > 10) {
      return;
    }

    queue[index++] = data;
  }

  byte pop() {
    byte res = queue[0];

    for (byte i = 1; i < 10; i++) {
      queue[i - 1] = queue[i];
    }

    index--;

    return res;
  }

  ~Queue();
};