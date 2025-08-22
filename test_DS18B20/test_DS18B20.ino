#include <OneWire.h>
#include <DallasTemperature.h>

#define DQ_Pin 2

OneWire oneWire(DQ_Pin);
DallasTemperature sensors(&oneWire);

void setup(void) {
  Serial.begin(9600);
  sensors.begin();
}

void loop(void) {
  Serial.print("Temperatures --> ");
  sensors.requestTemperatures();
  Serial.println(sensors.getTempCByIndex(0));
  delay(2000);
}