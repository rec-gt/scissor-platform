byte sensorPins[] = {
  A0,
  A1,
  A2,
  A3,
  A4,
  A5,
  A6,
  A7,
  A8,
  A9,
}

LaserSensor sensors[] = {
  // (longer threshold, shorter threshold, fine tune)
  LaserSensor(A0, 800, 500, 203),  // 工廠fine-tune，遮擋鏡頭做測試
  LaserSensor(A1, 800, 500, 216),
  LaserSensor(A2, 800, 500, 215),
  LaserSensor(A3, 800, 500, 232),
  LaserSensor(A4, 800, 500, 222),
  LaserSensor(A5, 800, 500, 216),
  LaserSensor(A6, 800, 500, 234),
  LaserSensor(A7, 800, 500, 229),
  LaserSensor(A8, 800, 500, 227),
  LaserSensor(A9, 800, 500, 222),
};

void setup() {
}