// #include "NBIoT_Async.h";
#include "NBIoT.h";

NBIoT nbiot;

void setup() {
  Serial.begin(9600);
  Serial3.begin(9600);
  nbiot.init();
}

void loop() {
  // if (Serial3.available()) {
  //   String str = Serial3.readString();
  //   Serial.print(str);
  //   nbiot.sendCMD(str);
  //   delay(1000);
  // }

  nbiot.sendCMD("AT+MQTTPUB=\"rgt/869976034806621/chirpstack\",1,0,0,0,\"{\"code\":\"Tim-Test\"\t,\"sensors\":[1700,1700,1700,1700,1700,1700,1700,1700,1700,1700,1700]\t}\"");

  delay(1000);


  // nbiot.sendCMD("AT+MQTTPUB=\"rgt/869976034806621/chirpstack\",1,0,0,0,\"{\"code\":\"Tim-Test\"\t,\"sensors\":[1700,1700,1700,1700,1700,1700,1700,1700,1700,1700,1700]\t}\"");
  // delay(30 * 1000);
  // nbiot.sendCMD("AT+MQTTPUB=\"rgt/869976034806621/in\",1,0,0,0,");
  // delay(30 * 1000);

  // { "seq": }
  // IOT_3("\"{\"seq\":%lu,\"csq\":%u,\"sw\":%u,", 0, csq, SW_Reg);
  // IOT_2("\"din\":%u,\"dout\":%u,", DI_Reg, DO_Reg);
  // IOT_4("\"ain\":[%u,%u,%u,%u],", AI_Reg[0], AI_Reg[1], AI_Reg[2], AI_Reg[3]);
  // IOT_4("\"aout\":[%u,%u,%u,%u]}\"\r\n", AO_Reg[0], AO_Reg[1], AO_Reg[2], AO_Reg[3]);
}