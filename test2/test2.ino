// ADC Oversampling to 14-bit with Smoothing
// For Arduino boards with 10-bit ADC (e.g., Uno, Nano)

// To achieve 14-bit resolution, we need 4^4 = 256 samples per reading
#define OVERSAMPLE_COUNT 256
#define SMOOTHING_SAMPLES 32  // Number of readings for moving average smoothing

// Array to store smoothing samples
long smoothingBuffer[SMOOTHING_SAMPLES];
int smoothingIndex = 0;
long smoothingSum = 0;

void setup() {
  // Initialize serial communication
  Serial.begin(9600);

  // Set ADC reference to default (5V for most Arduino boards)
  analogReference(EXTERNAL);

  // Initialize smoothing buffer
  for (int i = 0; i < SMOOTHING_SAMPLES; i++) {
    smoothingBuffer[i] = 0;
  }
}

long readADC14Bit(int pin) {
  long sum = 0;

  // Take 256 samples for 14-bit oversampling
  for (int i = 0; i < OVERSAMPLE_COUNT; i++) {
    sum += analogRead(pin);
  }

  // Divide by 16 to get 14-bit resolution (1024 * 256 / 16 = 16384 levels)
  return (sum >> 4);
}

long smoothADC(long newReading) {
  // Subtract the oldest reading
  smoothingSum -= smoothingBuffer[smoothingIndex];

  // Add the new reading
  smoothingSum += newReading;
  smoothingBuffer[smoothingIndex] = newReading;

  // Update index
  smoothingIndex = (smoothingIndex + 1) % SMOOTHING_SAMPLES;

  // Return the average
  return smoothingSum / SMOOTHING_SAMPLES;
}

void loop() {
  // Read from analog pin A0
  long raw14Bit = readADC14Bit(A2);
  long smoothedValue = smoothADC(raw14Bit);

  // Print results
  Serial.print("14-bit Raw: ");
  Serial.print(raw14Bit);
  Serial.print(" | Smoothed: ");
  Serial.print(smoothedValue);
  Serial.print(" | Mapped: ");

  if (smoothedValue <= 1593) {
    Serial.println(map(smoothedValue, 0, 1593, 0, 2000));
  } else {
    Serial.println(map(smoothedValue, 1594, 8067, 2001, 4096));
  }

  // if (smoothedValue <= 1593) {
  //   Serial.println(map(smoothedValue, 0, 1593, 0, 2000));
  // } else if (smoothedValue <= 3211) {
  //   Serial.println(map(smoothedValue, 1594, 3211, 2001, 4000));
  // } else if (smoothedValue <= 4827) {
  //   Serial.println(map(smoothedValue, 3211, 4827, 4001, 6000));
  // } else if (smoothedValue <= 6451) {
  //   Serial.println(map(smoothedValue, 4828, 6451, 6001, 8000));
  // } else {
  //   Serial.println(map(smoothedValue, 6452, 8068, 8001, 10000));
  // }
}