class SlaveData {
private:
  uint8_t id;
  String name = "iFCU-16";
  bool onOff = false;
  uint8_t mode = 0;
  uint8_t speed = 0;
  uint16_t roomTemp = 2200;
  uint16_t setTemp = 2500;
  uint16_t lowerTempLimit = 1500;
  uint16_t higherTempLimit = 2700;

public:
  String dataStr = "";

  SlaveData() {}

  void set(
    uint8_t id,
    String name,
    bool onOff,
    uint8_t mode,
    uint8_t speed,
    uint16_t roomTemp,
    uint16_t setTemp,
    uint16_t lowerTempLimit,
    uint16_t higherTempLimit) {

    this->id = id;
    this->name = name;
    this->onOff = onOff;
    this->mode = mode;
    this->speed = speed;
    this->roomTemp = roomTemp;
    this->setTemp = setTemp;
    this->lowerTempLimit = lowerTempLimit;
    this->higherTempLimit = higherTempLimit;

    dataStr = "";
    dataStr += String(id);
    dataStr += String(",");
    dataStr += String(name);
    dataStr += String(",");
    dataStr += String(onOff);
    dataStr += String(",");
    dataStr += String(mode);
    dataStr += String(",");
    dataStr += String(speed);
    dataStr += String(",");
    dataStr += String(roomTemp);
    dataStr += String(",");
    dataStr += String(setTemp);
    dataStr += String(",");
    dataStr += String(lowerTempLimit);
    dataStr += String(",");
    dataStr += String(higherTempLimit);
  };

  ~SlaveData() {}
};
