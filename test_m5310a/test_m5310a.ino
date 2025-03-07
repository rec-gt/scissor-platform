#include "NBIoT.h";

// int socketid = 1;                 //socket端口编号
// #define KEEPALIVEATIME 30 * 1000  //心跳间隔，默认30秒发一次心跳
// unsigned long preHeartTick = 0;   //心跳时间
// #define TIMEOUT 3000              //接收esp8266反馈的超时时间

NBIoT nbiot;

void setup() {
  Serial.begin(9600);
  nbiot.init();
}


void loop() {
  nbiot.sendCMD("AT+MQTTPUB=\"rgt/869976034806621/sys\",1,0,0,0,\"{\"code\":\"Tim-Test\"\t,\"sensors\":[1700,1700,1700,1700,1700,1700,1700,1700,1700,1700,1700]\t}\"");
  // String IncomingString = "";                   //用于接收串口发来的数据
  // bool StringReady = false;                     //接收到串口数据的标志
  // while (NBIoTModule.available()) {             //如果接收到esp8266的数据
  //   IncomingString = NBIoTModule.readString();  //获取esp8266反馈的数据，及esp8266收到远程服务器发来的数据
  //   StringReady = true;                         //接收到数据的标志
  // }

  // if (StringReady) {                                       //如果有数据发来，检查接收到的数据
  //   Serial.println("Received String: " + IncomingString);  //串口打印显示收到的数据
  //   check_msg(IncomingString);                             //调用检查数据函数，进行检查数据
  // }

  // if (millis() - preHeartTick >= KEEPALIVEATIME) {                                                        //定时函数，用于保持心跳，30秒检测一次（现在时间减去上次时间是否大于或等于30s）
  //   preHeartTick = millis();                                                                              //获取现在时间，用于下次计算
  //   while (SendCommand("AT+NSOSD=" + String(socketid) + ",1,\"cmd=0&msg=keep\\r\\n\",,4", "OK", 1500)) {  //发送心跳
  //     delay(2000);                                                                                        //如果发送失败，延迟2秒后继续发送
  //     errorFlag++;                                                                                        //累加错误次数
  //     if (errorFlag >= 2) {                                                                               //如果错误达到2次
  //       errorFlag = 0;                                                                                    //清空错误次数
  //       NBIoTModule.println("AT+NSOCL=" + String(socketid));                                              //创建TCP连接
  //       socketid++;
  //       if (socketid >= 7) {
  //         socketid = 1;
  //       }
  //       InitNBIOT();  //重新连接网络
  //     }
  //   }
  //   errorFlag = 0;
  // }
}




// bool SendCommand(String cmd, String ack, int timeout) {
//   NBIoTModule.println(cmd);     // 向软串口发送指令
//   Serial.println(cmd);          // 串口调试助手打印指令信息
//   if (!echoFind(ack, timeout))  // 如果超时或者错误响应
//   {
//     return true;  // 返回真，说明指令发送失败
//   } else {
//     return false;  //指令发送成功
//   }
// }

// bool echoFind(String keyword, int TimeOut) {
//   long deadline = millis() + TimeOut;      //设置超时时间
//   String get_msg = "";                     //用于接收软串口数据
//   while (millis() < deadline) {            //设置检测软串口时间
//     if (NBIoTModule.available()) {         //如果软串口有数据
//       get_msg = NBIoTModule.readString();  //读取软串口数据
//     }
//   }
//   if (get_msg != "") {                             //如果接收到数据
//     Serial.println(get_msg);                       //串口打印收到的数据
//     int keyword_index = get_msg.indexOf(keyword);  //获取关键字所在字符串位置
//     if (keyword_index >= 0) {                      //如果接收到的字符串有期待接收到的关键字
//       return true;                                 //返回真
//     }
//   }
//   return false;  // 超时或无检测数据
// }

/*
 * 初始化M5310
 * 创建TCP连接，并且订阅巴法云
 */
// void InitNBIOT(void) {
//   delay(500);
//   while (NBIoTModule.read() >= 0) {}                 //清空软串口数据，等待下次接收
//   NBIoTModule.println("AT+NSOCR=\"STREAM\",6,0,2");  //创建TCP连接
//   delay(2000);

//   while (SendCommand("AT+CIPSTART=TCP,bemfa.com,8344", "CONNECT", 3500)) {  //连接巴法云服务器
//     delay(2000);
//     Serial.println("init error");     //如果错误返回，等待1秒重新发送AT
//     errorFlag++;                      //记录错误次数
//     if (errorFlag >= 3) {             //如果连续错误5次，执行重启
//       NBIoTModule.println("AT+NRB");  //M5310重启指令，重启NB模块，适用于海思系列模块
//       delay(200);
//       resetFunc();  //重启函数，执行重启arduino
//     }
//   }
//   errorFlag = 0;                      //错误次数清零
//   while (NBIoTModule.read() >= 0) {}  //清空软串口数据，等待下次接收
//   delay(1000);
//   NBIoTModule.println("AT+NSOCFG=" + String(socketid) + ",0,0");  //设置字符串传输模式，1代表socket1,第一个0代表字符串模式接收，第二个0代表转义字符模式发送
//   delay(300);
//   // 发送订阅指令，如需订阅多个多个主题，可延迟一秒后继续发送订阅
// }
