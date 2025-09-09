// laser sensors pins
const int SENSOR_PINS[] = { A0, A1, A2, A3, A4, A5, A6, A7,  };
const int SENSORS_NUM = 10;

const float SENSOR_DISTANCE_BUFFER[] = { 100, 100, 100, 100, 100, 100, 100, 100, 100, 100 };
const float BASE_DISTANCE = 500;

const int BUTTON_PIN = 2;
const int RELAY_PIN = 4;
const int POWER_LIGHT_PIN = 6;
const int WARNING_LIGHT_PIN = 8;
const int SPEAKER_PIN = 10;
const int SWITCH_PIN = 12;

int previousSwitchStatus = LOW;

enum SystemStatus { 
  STOPPED,
  RUNNING,
  ALLOW_10S,
};

const char* STATUS_STR[] = { "STOPPED", "RUNNING", "ALLOW_10S" };

SystemStatus systemStatus = RUNNING;


// init timer
const unsigned long TIMER_DURATION = 10 * 1000;
unsigned long startMillis = millis();
int remainingTime = TIMER_DURATION / 1000;

void setup() {
  // init serial communication
  Serial.begin(9600);

  // init inputs
  pinMode(BUTTON_PIN, INPUT);

  // init outputs
  pinMode(RELAY_PIN, OUTPUT);
  pinMode(POWER_LIGHT_PIN, OUTPUT);
  pinMode(WARNING_LIGHT_PIN, OUTPUT);
  pinMode(SPEAKER_PIN, OUTPUT);

  // init system
  handlePowerLight();
  handleChangeSystemStatus(RUNNING);

  // init msg
  Serial.println(digitalRead(BUTTON_PIN));


  Serial.println("--- System Start ---");
  printSystemStatus();
}

void loop() {
  if (systemStatus == RUNNING) {
    handleRelay(true);
    listenSensors();
  }

  if (systemStatus == STOPPED) {
    handleRelay(false);
    handleWarningLight(true);
    handleSpeaker(true);
    listenButton();
  }

  if (systemStatus == ALLOW_10S) {
    handleRelay(true);
    handleWarningLight(false);
    handleSpeaker(false);
    handleTimer();
  }

  delay(10);
}

void printSystemStatus() {
  Serial.print("System current status: ");
  Serial.println(STATUS_STR[systemStatus]);
}

void printDistanceData(int i, float distance) {
  Serial.print("Sensor ");
  Serial.print(i);
  Serial.print(" - Distance: ");
  Serial.print(distance);
  Serial.println(" cm");
}

float getThresholdDistance(float bufferDistance) {
  return BASE_DISTANCE + bufferDistance;
}

void handleChangeSystemStatus(SystemStatus status) {
  systemStatus = status;

  Serial.print("System switch to '");
  Serial.print(STATUS_STR[status]);
  Serial.println("' status");
}

void handlePowerLight() {
  digitalWrite(POWER_LIGHT_PIN, HIGH);
}

void handleRelay(bool toggle) {
  // Relay default NO
  // set relay to true means give 5v to it, making the relay "connected"
  // set relay to false means give 0v to it, making the relay "disconnected"
  digitalWrite(RELAY_PIN, toggle ? HIGH : LOW);
}

void handleWarningLight(int toggle) {
  digitalWrite(WARNING_LIGHT_PIN, toggle ? HIGH : LOW);
}

void handleSpeaker(int toggle) {
  digitalWrite(SPEAKER_PIN, toggle ? HIGH : LOW);
}

void listenSensors() {
  // for (int i = 0; i < SENSORS_NUM; i++) {
  //   int sensorValue = analogRead(SENSOR_PINS[i]);
  //   float measuredDistance = sensorValue * 5.0 / 1023.0;

  //   if (measuredDistance > getThresholdDistance(SENSOR_DISTANCE_BUFFER[i])) {
  //     handleChangeSystemStatus(STOPPED);
  //   }
  // }
  delay(3000);
  Serial.println("Obstacle detected!");
  handleChangeSystemStatus(STOPPED);
}

void listenButton() {
  if (digitalRead(BUTTON_PIN) == LOW) {
    Serial.println("Allow Running for 10s");
    startMillis = millis();
    handleChangeSystemStatus(ALLOW_10S);
  }
}

void listenSwitch() {
  int currentSwitchState = digitalRead(SWITCH_PIN);  // Read the current state of the switch

  if (currentSwitchState != previousSwitchStatus) {
    if (currentSwitchState == HIGH) {
      Serial.println("Switch is ON");
    } else {
      Serial.println("Switch is OFF");
    }
    previousSwitchStatus = currentSwitchState;  // Update the previous state
  }
}

void handleTimer() {
  unsigned long currentMillis = millis();

  // print countdown
  int currentRemainingTime = (TIMER_DURATION - (currentMillis - startMillis)) / 1000;
  if (currentRemainingTime >= 0 && currentRemainingTime != remainingTime) {
    remainingTime = currentRemainingTime;
    Serial.println(int(currentRemainingTime) + 1);
  }

  if (currentMillis - startMillis >= TIMER_DURATION) {
    remainingTime = TIMER_DURATION / 1000;
    handleChangeSystemStatus(RUNNING);
  }
}
