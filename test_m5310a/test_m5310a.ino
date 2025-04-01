// #include "NBIoT_Async.h";
#include "NBIoT.h";

NBIoT nbiot;

void setup() {
  Serial.begin(9600);
  Serial1.begin(9600);
  nbiot.init();
}

void loop() {
  nbiot.sendCMDFast("AT+MQTTPUB=\"rgt/869976034840018/in\",1,0,0,0,\"{\"seq\":1,\"csq\":25,\"sw\":0,\"din\":" + String(255) + ",\"dout\":" + String(255) + ",\"ain\":[0,0,0,0],\"aout\":[0,0,0,0]}\"");
  delay(30 * 1000);
}
