#include <WiFi.h>
#include <HTTPClient.h>
#include <EEPROM.h>
#include <ArduinoJson.h>
#include "iFCUModbus.h"
#define EEPROM_SIZE 1024

const char* ssid = "REC Guest";        // Enter SSID here
const char* password = "guest@@2022";  // Enter Password here

HTTPClient http;

constexpr size_t DEVICE_COUNT = 32;
constexpr size_t HR_FIELD_SIZE = 4;
constexpr size_t IR_FIELD_SIZE = 5;
uint16_t hrDatabase[DEVICE_COUNT][HR_FIELD_SIZE];
uint16_t irDatabase[DEVICE_COUNT][IR_FIELD_SIZE];

String getReqGetHR = "http://10.236.207.100:3000/broker/get-hr/1";
// String getReqSetHR = "http://10.236….207.100:3000/broker/set-hr-device";
String postReqSetIR = "http://10.236.207.100:3000/broker/set-ir";

constexpr size_t arrayLength = 4 + 1;
uint16_t jsArray[arrayLength];

iFCUModbus ifcuModbus;

void setup() {
  EEPROM.begin(EEPROM_SIZE);

  Serial.begin(115200);

  Serial.println("Connecting to ");
  Serial.println(ssid);

  //connect to your local wi-fi network
  WiFi.begin(ssid, password);

  //check wi-fi is connected to wi-fi network
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected..!");
  Serial.print("Got IP: ");
  Serial.println(WiFi.localIP());

  ifcuModbus.init();
}

void loop() {
  handleGetAndSetHR();
  delay(1000);
  handleSendHR();
  delay(1000);
}

void handleFetchAll() {
  String str = "";

  str += "{";
  str += "HR:[";
  for (size_t i = 0; i < DEVICE_COUNT; i++) {
    str += "[";
    for (size_t j = 0; j < HR_FIELD_SIZE; j++) {
      str += hrDatabase[i][j];
      str += ",";
    }
    str += "],";
  }
  str += "],";
  str += "IR:[";
  for (size_t i = 0; i < DEVICE_COUNT; i++) {
    str += "[";
    for (size_t j = 0; j < IR_FIELD_SIZE; j++) {
      str += irDatabase[i][j];
      str += ",";
    }
    str += "],";
  }
  str += "],";
  str += "}";


  Serial.println(str);
}


void handleGetAndSetHR() {
  http.begin(getReqGetHR);
  http.addHeader("Content-Type", "application/x-www-form-urlencoded");

  int httpResponseCode = http.GET();
  if (httpResponseCode > 0) {
    Serial.print("HTTP Response Code: ");
    Serial.println(httpResponseCode);
    String payload = http.getString();
    if (payload) {

      int startIndex = 1;  // Skip the opening bracket '['
      int endIndex = payload.indexOf(',', startIndex);
      for (int i = 0; i < arrayLength; i++) {
        if (endIndex == -1) {
          endIndex = payload.indexOf(']', startIndex);
        }
        jsArray[i] = payload.substring(startIndex, endIndex).toInt();
        startIndex = endIndex + 1;  // Skip the comma and space
        endIndex = payload.indexOf(',', startIndex);
      }

      if (jsArray[0] == 1) {
        Serial.println("Parsed array:");
        for (int i = 0; i < arrayLength; i++) {
          Serial.print(jsArray[i]);
          Serial.print(" ");
        }

        node.setTransmitBuffer(0, jsArray[1]);
        node.setTransmitBuffer(1, 0);
        node.setTransmitBuffer(2, jsArray[3]);
        node.setTransmitBuffer(3, jsArray[4]);
        node.setTransmitBuffer(4, jsArray[2]);
        result = node.writeMultipleRegisters(40000, 5);
        if (result == node.ku8MBSuccess) {
          Serial.println("Success");
        } else {
          Serial.println("Fail to update data");
        }
      }
    }

  } else {
    Serial.print("Error code: ");
    Serial.println(httpResponseCode);
  }

  http.end();
}

void handleSendHR() {
  result = node.readHoldingRegisters(40000, 5);
  if (result == node.ku8MBSuccess) {
    for (size_t i = 0; i < 5; i++) {
      HR_DATABASE[i] = node.getResponseBuffer(i);
      Serial.println(HR_DATABASE[i]);
    }
  } else {
    Serial.println("Cannot Fetch Data");
  }

  // result = node.readInputRegisters(30000, IR_SIZE);
  // if (result == node.ku8MBSuccess) {
  //   for (size_t i = 0; i < IR_SIZE; i++) {
  //     IR_DATABASE[i] = node.getResponseBuffer(i);
  //     Serial.println(IR_DATABASE[i]);
  //   }
  // } else {
  //   Serial.println("Cannot Fetch Data");
  // }
  String baseURL = "http://10.236.207.100:3000/broker/set-hr-device";

  String powerParam = "power=" + String(HR_DATABASE[0]);
  String setTempParam = "setTemp=" + String(HR_DATABASE[4]);
  String modeParam = "mode=" + String(HR_DATABASE[2]);
  String speedParam = "speed=" + String(HR_DATABASE[3]);

  String getReqSetHR = baseURL + "?" + powerParam + "&" + setTempParam + "&" + modeParam + "&" + speedParam;

  http.begin(getReqSetHR);
  Serial.println(getReqSetHR);
  http.addHeader("Content-Type", "application/x-www-form-urlencoded");
  int httpResponseCode = http.GET();
  Serial.println(httpResponseCode);
  http.end();
}
