#include "Arduino.h"
#include "LaserSensor.h"
#include "DownwardSensor.h"

// 工廠fine-tune, 把所有sensors蓋住，然後測試0cm reading
LaserSensor sensors[] = {
  LaserSensor(A0, 210),
  LaserSensor(A1, 208),
  LaserSensor(A2, 211),
  LaserSensor(A3, 222),
  LaserSensor(A4, 213),
  LaserSensor(A5, 207),
  LaserSensor(A6, 232),
  LaserSensor(A7, 225),
  LaserSensor(A8, 219),
  LaserSensor(A9, 214),
};

LaserSensorManager manager(sensors, sizeof(sensors) / sizeof(sensors[0]));

DownwardSensor downwardSensor(A11, 305);

void setup() {
  Serial.begin(9600);
  analogReference(EXTERNAL);
  // === 10 Sensors ===
  manager.calibrateAllReading();  // 第一步，然後更新上面的reading，再進行第二步
  // manager.calibrateAllDistance();  // 第二步，誤差在+-10mm都可以

  // === Downward Sensors ===
  // downwardSensor.calibrateReading();
}

void loop() {
  // downwardSensor.print();
  delay(300);
};