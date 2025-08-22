#ifndef Globals_H
#define Globals_H


// Serial.println(map(analogInputs[i].getValue(), 0, 16368, 0, 50000));// for 0-5V 經驗數值
// Serial.println(map(analogInputs[i].getValue(), 3360, 16368, 10000, 50000)); // for 4-20mA 經驗數值

/*=== For NBIoT ===*/
String serialRes = "";
String connStr = "";
String subStr = "";
String subRecvContent = "";
String pubMsgContent = "";
String pubMsgPrepare = "";
String pubMsgForce = "";

#endif