class SlaveData {
public:
  uint8_t id;
  char* name = "iFCU-16";
  bool onOff = false;
  uint8_t mode = 0;
  uint8_t speed = 0;
  uint16_t roomTemp = 2200;
  uint16_t setTemp = 2500;
  uint16_t lowerTempLimit = 1500;
  uint16_t higherTempLimit = 2700;

  SlaveData() {}
  ~SlaveData() {}
};
