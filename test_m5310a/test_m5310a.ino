#define NBIoTModule Serial1

// int socketid = 1;                 //socket端口编号
// #define KEEPALIVEATIME 30 * 1000  //心跳间隔，默认30秒发一次心跳
// unsigned long preHeartTick = 0;   //心跳时间
// #define TIMEOUT 3000              //接收esp8266反馈的超时时间

class NBIoT {
private:
  byte errCount = 0;
  bool received = false;
  String response = "";  // 節省空間
  char* res = "";        // 節省空間
  bool canGoNext = false;

  void clearBuffer() {
    while (NBIoTModule.read() >= 0) {}
  }

  bool resContain(const char* target) {
    for (int i = 0; this->res[i] != '\0'; i++) {
      bool found = true;
      for (int j = 0; target[j] != '\0'; j++) {
        if (this->res[i + j] != target[j]) {
          found = false;
          break;
        }
      }
      if (found) {
        return true;
      }
    }
    return false;
  }

  bool parseResponse(char* res) {
    return this->resContain("OK");
  }

  void str2Char() {
    this->res = const_cast<char*>(this->response.c_str());
  }

  void tryComm(void (*callback)(), bool (*breakCondition)(), uint32_t interval = 3000) {
    while (true) {
      callback();
      if (breakCondition()) {
        break;
      }
      delay(interval);
    }
  }

  void connectToNetwork() {
    this->clearBuffer();

    bool goNext = false;

    // ask for 9600 baud rate
    while (1) {
      this->sendCMD("AT+NATSPEED=9600,30,0,0");
      if (this->resContain("OK")) {
        break;
      }
      delay(3000);
    }

    // check communication success
    while (1) {
      this->sendCMD("AT");
      if (this->resContain("OK")) {
        break;
      }
      delay(3000);
    }

    // get cimi
    while (1) {
      this->sendCMD("AT+CIMI");
      if (this->resContain("OK")) {
        break;
      }
      delay(3000);
    }

    while (1) {
      this->sendCMD("AT+CSQ");
      if (this->resContain("OK")) {
        break;
      }
      delay(3000);
    }

    while (1) {
      this->sendCMD("AT+CEREG?");
      if (this->resContain("OK")) {
        break;
      }
      delay(3000);
    }

    while (1) {
      this->sendCMD("AT+CEREG=1");
      if (this->resContain("OK")) {
        break;
      }
      delay(3000);
    }

    while (1) {
      this->sendCMD("AT+CGATT?");
      if (this->resContain("OK")) {
        break;
      }
      delay(3000);
    }

    while (1) {
      this->sendCMD("AT+CGSN=1");
      if (this->resContain("OK")) {
        break;
      }
      delay(3000);
    }

  


    // nbiot.sendCMD("AT+MQTTDISC");

    // nbiot.sendCMD("AT+MQTTDEL");

    // nbiot.sendCMD("AT+MQTTCFG=\"iot.rec-gt.com\",1880,\"869976034806621\",60,\"tswh\",\"1Wo=[6vA0m\",1");

    // nbiot.sendCMD("AT+MQTTOPEN=1,1,1,0,1,\"rgt/869976034806621/dev\",\"gone\"");
  }

public:
  NBIoT(){};

  void init() {
    NBIoTModule.begin(9600);
    this->connectToNetwork();
  }

  char* concatCharN(char** charArr, size_t arrSize) {
    int totalCharLen = 0;
    for (size_t i = 0; i < arrSize; i++) {
      totalCharLen += strlen(charArr[i]);
    }
    char* newChar = new char[totalCharLen + 1];

    newChar[0] = '\0';

    for (size_t i = 0; i < arrSize; i++) {
      strcat(newChar, charArr[i]);
    }

    return newChar;
  }

  bool sendCMD(String cmd, uint32_t timeout = 1000) {
    unsigned long deadline = millis() + timeout;  // max = 24*60*60*1000 (86400000 / 1day), default 1s
    NBIoTModule.println(cmd);

    while (millis() < deadline) {
      if (NBIoTModule.available()) {
        this->response = NBIoTModule.readString();
        this->str2Char();
        Serial.print(cmd + ": ");
        Serial.println(this->response);
        this->clearBuffer();
        return true;
      }
    }
    this->clearBuffer();
    return false;
  }

  ~NBIoT(){};
};

NBIoT nbiot;

void setup() {
  Serial.begin(9600);
  Serial.println("Entering Loop");

  nbiot.init();
}


void loop() {
  // nbiot.sendCMD("AT+MQTTPUB=\"rgt/869976034806621/sys\",1,0,0,0,\"{\"code\":\"Tim-Test\"\t,\"sensors\":[1700,1700,1700,1700,1700,1700,1700,1700,1700,1700,1700]\t}\"");

  // nbiot.sendCMD("AT+MQTTSUB=\"rgt/869976034806621/out\",1,0");

  delay(10 * 60 * 1000);
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
