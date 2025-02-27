String uid = "e56277d3116647938af97cda298b066e";  //用户私钥，巴法云控制台获取
String myTopic = "test123";                       //用户主题，巴法云控制台创建


void setup() {
  Serial.begin(9600);
}

void loop() {
  Serial.println("data1001");
  void check_msg(String myMsg) {
    if ((myMsg.indexOf("&msg=on") >= 0)) {          //如果检测到开灯指令
      turnOnLed();                                  //执行开灯函数
    } else if ((myMsg.indexOf("&msg=off") >= 0)) {  //如果检测到关灯指令
      turnOffLed();                                 //执行关灯函数
    } else if (myMsg.indexOf("SOCLI") >= 0) {       //检测到断开服务器连接，重新连接
      InitNBIOT();                                  //创建TCP连接，并且订阅巴法云
    }
  }
  delay(1000);
}
