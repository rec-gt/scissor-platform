#ifndef Lock_H
#define Lock_H

class Lock {
private:
  bool _lock = false;

public:
  Lock(){};

  void lock() {
    this->_lock = true;
  }

  bool isLocked() {
    return this->_lock;
  }

  void release() {
    this->_lock = false;
  }

  bool isReleased() {  // for better semantic understanding, "!isLocked()" is not explicit.
    return !this->_lock;
  }

  ~Lock() {}
};

#endif