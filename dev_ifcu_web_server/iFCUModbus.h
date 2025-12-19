#ifndef IFCU_MODBUS_H
#define IFCU_MODBUS_H

#include "Globals.h"

#define RXD2 16
#define TXD2 17

constexpr size_t arrayLength = 4 + 1;
uint16_t jsArray[arrayLength];

uint8_t result;

class iFCUModbus {
private:
  uint16_t prevMillis = millis();
  byte errMsg = 0;

public:
  void init() {
    Serial2.begin(9600, SERIAL_8N1, RXD2, TXD2);
    mbNode.begin(IFCU_SLAVE_ID, Serial2);
  }

  void handleGetAndSetHR() {
    httpReqGetHR = F("http://");
    httpReqGetHR += gatewayIPAddress;
    httpReqGetHR += F(":3000/broker/get-hr/");
    httpReqGetHR += DEVICE_ID;

    http.begin(httpReqGetHR);
    http.addHeader(F("Content-Type"), F("application/x-www-form-urlencoded"));
    Serial.println(httpReqGetHR);

    int httpResponseCode = http.GET();
    if (httpResponseCode > 0) {
      Serial.println(httpResponseCode);
      String payload = http.getString();
      if (payload) {
        int startIndex = 1;
        int endIndex = payload.indexOf(',', startIndex);
        for (int i = 0; i < arrayLength; i++) {
          if (endIndex == -1) {
            endIndex = payload.indexOf(']', startIndex);
          }
          jsArray[i] = payload.substring(startIndex, endIndex).toInt();
          startIndex = endIndex + 1;
          endIndex = payload.indexOf(',', startIndex);
        }

        if (jsArray[0] == 1) {
          Serial.println("Parsed array:");
          for (int i = 0; i < arrayLength; i++) {
            Serial.print(jsArray[i]);
            Serial.print(" ");
          }

          mbNode.setTransmitBuffer(0, jsArray[1]);
          mbNode.setTransmitBuffer(1, 0);
          mbNode.setTransmitBuffer(2, jsArray[3]);
          mbNode.setTransmitBuffer(3, jsArray[4]);
          mbNode.setTransmitBuffer(4, jsArray[2]);
          result = mbNode.writeMultipleRegisters(40000, 5);
          if (result == mbNode.ku8MBSuccess) {
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

  void handleSendIR() {
    result = mbNode.readInputRegisters(30000, IR_SIZE);

    if (result == mbNode.ku8MBSuccess) {
      for (size_t i = 0; i < IR_SIZE; i++) {
        IR_DATABASE[i] = mbNode.getResponseBuffer(i);
      }
      this->errMsg = 0;
    } else {
      this->errMsg = 1;
      Serial.println("Cannot Fetch Data");
    }

    httpReqSetHR = F("http://");
    httpReqSetHR += gatewayIPAddress;
    httpReqSetHR += F(":3000/broker/set-hr-device");

    String idParam = "id=" + String(DEVICE_ID);
    String powerParam = "power=" + String((IR_DATABASE[1] & 0b01000000) != 0);
    String roomTempParam = "roomTemp=" + String(IR_DATABASE[5]);
    String setTempParam = "setTemp=" + String(IR_DATABASE[6]);
    String modeParam = "mode=" + String(IR_DATABASE[3]);
    String speedParam = "speed=" + String(IR_DATABASE[4]);

    httpReqSetHR += F("?");
    httpReqSetHR += idParam;
    httpReqSetHR += F("&");
    httpReqSetHR += powerParam;
    httpReqSetHR += F("&");
    httpReqSetHR += roomTempParam;
    httpReqSetHR += F("&");
    httpReqSetHR += setTempParam;
    httpReqSetHR += F("&");
    httpReqSetHR += modeParam;
    httpReqSetHR += F("&");
    httpReqSetHR += speedParam;
    httpReqSetHR += F("&");
    httpReqSetHR += F("err=");
    httpReqSetHR += this->errMsg;

    http.begin(httpReqSetHR);
    http.addHeader(F("Content-Type"), F("application/x-www-form-urlencoded"));
    Serial.println(httpReqSetHR);

    int httpResponseCode = http.GET();
    Serial.println(httpResponseCode);

    http.end();
  }
};

extern iFCUModbus ifcuModbus;

#endif