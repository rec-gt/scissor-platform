class LaserSensorManager {
private:
  LaserSensor* laserSensors;
  byte num;
public:
  LaserSensorManager(LaserSensor* sensors, byte num) {
    this->laserSensors = sensors;
    this->num = num;
    for (int i = 0; i < num; i++) {
      this->laserSensors[i] = sensors[i];
    }
  };

  void print() {
    Serial.println(num);
  }
};