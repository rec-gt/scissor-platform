#include <OneWire.h>
#include <DallasTemperature.h>

#define DQ_Pin 3

OneWire oneWire1(2);
OneWire oneWire2(3);
DallasTemperature sensors1(&oneWire1);
DallasTemperature sensors2(&oneWire2);

void setup(void) {
  Serial.begin(9600);
  sensors1.begin();
  sensors2.begin();
  pinMode(A0, INPUT);
}

void loop(void) {
  Serial.println("====");

  // Serial.print("Temp1 --> ");
  // sensors1.requestTemperatures();
  // Serial.print(sensors1.getTempCByIndex(0));
  // Serial.println();

  // Serial.print("Temp2 --> ");
  // sensors2.requestTemperatures();
  // Serial.print(sensors2.getTempCByIndex(0));

  // Serial.println();
  // Serial.println("====");

  Serial.print("Temp3 --> ");
  Serial.print(map(analogRead(A0), 0, 1023, 10, 100));

  delay(1000);
}