#ifndef Toggle_H
#define Toggle_H

class Toggle {
private:
  bool _toggle = false;

public:
  /* === for better semantic understanding, function in different name with the same usage are used. === */
  Toggle(){};

  /* === "T/F" Semantic === */
  void setToTrue() {
    this->_toggle = true;
  }

  void setToFalse() {
    this->_toggle = false;
  }

  bool isTrue() {
    return this->_toggle;
  }

  bool isFalse() {
    return !this->_toggle;
  }

  /* === "Lock" Semantic === */
  void lock() {
    this->_toggle = true;
  }

  void release() {
    this->_toggle = false;
  }

  bool isLocked() {
    return this->_toggle;
  }

  bool isReleased() {
    return !this->_toggle;
  }

  /* === "Mode" Semantic === */
  void on() {
    this->_toggle = true;
  }

  void off() {
    this->_toggle = false;
  }

  bool isOn() {
    return this->_toggle;
  }

  bool isOff() {
    return !this->_toggle;
  }

  ~Toggle() {}
};

#endif