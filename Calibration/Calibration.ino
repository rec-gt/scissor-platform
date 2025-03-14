#include "Arduino.h"
#include "LaserSensor.h"

LaserSensor sensors[] = {
  LaserSensor(A0, 203),
  LaserSensor(A1, 216),
  LaserSensor(A2, 215),
  LaserSensor(A3, 232),
  LaserSensor(A4, 222),
  LaserSensor(A5, 216),
  LaserSensor(A6, 234),
  LaserSensor(A7, 229),
  LaserSensor(A8, 227),
  LaserSensor(A9, 222),
};

LaserSensorManager manager(sensors, sizeof(sensors) / sizeof(sensors[0]));

void setup() {
  Serial.begin(9600);
  manager.calibrateAll();
}

void loop(){};