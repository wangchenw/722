/*
  名称：基于MQTT的万能AT指令（名称来源于本人名字故取名为WZ指令）

  **使用方法：
  普通服务器举例：
  {"wz":"wenzheng.club","server":"60.205.203.64","user":"admin1","password":"public","topic":"20190213134505","message":"test2019"}
  onenet 举例：
  {"wz":"wenzheng.club","id":"517254399","port":"6002","server":"mqtt.heclouds.com","user":"212737","password":"eL2ObaFLA1UvjAfxjCBcUYKjxtY=","topic&msg":"{\"wendu\":\"14\",\"shidu\":\"66\"}"}
  单片机需要转义：
  printf("{\"wz\":\"wenzheng.club\",\"server\":\"60.205.203.64\",\"user\":\"root\",\"password\":\"citc2018\",\"topic\":\"ZX10403001010000\",\"message\":\"T:226;H:232;\"}")
  **字段说明：
  server：ip或者域名
  port：端口（可不添加默认1883）
  user:用户名
  password：密码
  topic：发布的主题
  sub_topic：订阅的主题
  message：发布的消息
  topic&msg：onenet专属结构
  2020年2月优化4.0版本
  优化指示灯，采用定时器实现。

  版本：V3.8
  2019年7月24日 清除判网逻辑
  2019年6月22日00:15更新看门狗逻辑(看门狗默认开启)，断网处理逻辑，修复断网卡死，修复无限循环配网的问题，基本实现稳定运行。 WZ指令更换WIFI待优化。
  2019年4月18日更新sub_topic字段 用于用户自定义Topic
  更新：2019年4月11日修复onenet大于3个数据点内存溢出问题
       修复数据反馈异常，清除多余打印数据。
  特点：简单到只需要一行代码就可以将数据点推送至MQTT服务器
  可用于个人EMQ服务器，或者树莓派搭建的局域网服务器，已支持onenet等商用平台
  更新日志：
  2019年2月18日午夜更新V3.0版本
  支持onenet平台
  2019年2月13日晚上制作V2.0版本
  添加mqtt_user验证
  添加mqtt_passward验证
  2019年2月13日上午制作V1.0版本
  实现基本通讯
  2019年初有初步想法
  网站：http://wenzheng.club
  Github:https://github.com/az666
*/
#include <ESP8266WiFi.h>
#include <ArduinoJson.h>
#include <Wire.h>
#include <EEPROM.h>
#include <PubSubClient.h>
#include <Ticker.h>
WiFiClient espClient;
PubSubClient client(espClient);
Ticker flipper;
int flipper_count = 0;
struct CONFIG {  //结构体存放账号密码主题和消息
  char ssid[32];
  char password[32];
  char server[32];
  char topic[32];
  char sub_topic[32];
  char message[200];
  char onenet_topic_msg[200];
};
String payload = "";
String inputString = "";
String mqtt_user, sub_topic;
int port;
String mqtt_password;
String mqtt_id;
boolean stringComplete = false;
long lastMsg = 0;
int led_flag = 0;
int flip_flag = 0;
int wifi_err_time = 0;
char msg_buf[200];
char  c[] = "";
int server_flag = 0;
char msgJson[200];  //存储json数据
char debug_buf[200];  //打印调试数据
unsigned short json_len = 0;
void setup() {
  Serial.begin(115200);
  pinMode(2, OUTPUT);
  digitalWrite(2, HIGH);
  flipper.attach(1, flip); //100 ms 定时器
  setup_wifi();
  Serial.print("wifi_OK\r\n");
  Serial.print("please connect the server!\r\n");
  inputString.reserve(1024);
}
void loop() {
  Mqtt_Task();
  Serial_Task();
  Dog_Task();
}
/*********************************定时器回调**************************************/
void flip() {    //定时器回调
  digitalWrite(2, !digitalRead(2));
  switch (flip_flag) //判断标志位
  {
    case 0: //正常工作状态

      break;
    case 1: //发消息状态 快闪5次 恢复正常
      flipper_count++;
      if (flipper_count == 8)
      {
        flipper.attach(1, flip); //恢复工作状态
        flipper_count = 0;
      }
      break;
    case 2: //断网重连状态 一直闪
      //可以加更多逻辑
      break;
    case 3: //wifi连接超时开始配网
      //可以加更多逻辑
      break;
    default:
      //可以加更多逻辑
      break;
  }
}
/*********************************看门狗与自检**************************************/
void Dog_Task()
{
  long now_time = millis();

  if (now_time - lastMsg > 2000) {
    lastMsg = now_time;
    ESP.wdtFeed(); //喂狗
    if (!WiFi.isConnected()) //wifi异常
    {
      wifi_err_time += 1;
      if (wifi_err_time > 10)
      {
        if (!WiFi.isConnected()) //wifi异常
          ESP.restart();
      }
    } else
    {
      wifi_err_time = 0;
    }
  }
  if (digitalRead(0) == 0) //拉低主动进入配网模式
  {
    smartConfig();  //微信智能配网
  }

}
/*********************************消息回调**************************************/
void callback(char* topic, byte* payload, unsigned int length)  //接收消息
{
  Serial.print("WZ:[");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.print("]");
  Serial.print("\r\n");
}
String macToStr(const uint8_t* mac)
{
  String result;
  for (int i = 0; i < 6; ++i) {
    result += String(mac[i], 16);
    if (i < 5)
      result += ':';
  }
  return result;
}
/*********************************连接wifi**************************************/
int setup_wifi() {

  EEPROM.begin(512);
  CONFIG buf;
  EEPROM.get<CONFIG>(0, buf);
  delay(1000);
  Serial.println(buf.ssid);
  Serial.println(buf.password);
  //EEPROM.commit();
  delay(1000);
  WiFi.begin(buf.ssid, buf.password);
  long lastMsg = millis();
  while (WiFi.status() != WL_CONNECTED) {
    //正在连接wifi
    delay(500);
    //Serial.print(".");
    long now = millis();
    if (now - lastMsg > 10000) {
      //wifi 连接超时
      smartConfig();  //微信智能配网
      break;
    }
  }


}
/*********************************服务器连接**************************************/
void reconnect() {
  int connected_try_time = 0;
  while (!client.connected()) {
    flip_flag = 2; //切换为重连状态
    connected_try_time++;
    if (connected_try_time > 10) {
      connected_try_time = 0;
      ESP.restart();
    }
    CONFIG buf;
    String clientName;
    clientName += "esp8266-";
    uint8_t mac[6];
    WiFi.macAddress(mac);
    clientName += macToStr(mac);
    clientName += "-";
    clientName += String(micros() & 0xff, 16);
    if (mqtt_id == "") mqtt_id = clientName; //判断是否要添加ID
    if  (client.connect((char*)mqtt_id.c_str(), (char*)mqtt_user.c_str(), (char*)mqtt_password.c_str()))  {  //ID与账号和密码
      Serial.println("connected_OK");
      flip_flag = 1; //切换为正常状态
      client.subscribe(sub_topic.c_str(), 1); //为保证主题不受冲突 (const char*)
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());  //设备重连
      Serial.print(",try time=");
      Serial.print(connected_try_time);
      Serial.println(" try_to_connected");
    }
  }
}
/*********************************串口任务**************************************/
void Serial_Task() {
  while (Serial.available()) {
    char inChar = (char)Serial.read();
    inputString += inChar;
    if (inChar == '\n') {
      stringComplete = true;
      flip_flag = 1; //切换消息状态
      flipper.attach(0.1, flip);
    }
  }
}
/*********************************智能配网**************************************/
void smartConfig()
{
  Serial.println("SmartConfig begin");
  WiFi.mode(WIFI_STA);
  WiFi.beginSmartConfig();
  int try_time = 0;
  flip_flag = 3 ; //定时器标志位为3
  flipper.attach(0.1, flip); //快闪
  while (1)
  {
    try_time++;
    delay(500);
    if (try_time > 100) {
      try_time = 0;
      ESP.restart();
    }
    //Serial.print(".");
    flipper.attach(0.1, flip);
    if (WiFi.smartConfigDone())
    {
      EEPROM.begin(512);
      CONFIG buf;
      Serial.println("SmartConfig Success");
      flip_flag = 0; //配网成功 状态恢复
      flipper.attach(1, flip); //工作状态
      Serial.printf("SSID:%s\r\n", WiFi.SSID().c_str());
      Serial.printf("PSW:%s\r\n", WiFi.psk().c_str());
      strcpy(buf.ssid,  WiFi.SSID().c_str());
      strcpy(buf.password, WiFi.psk().c_str());
      EEPROM.put<CONFIG>(0, buf);
      EEPROM.commit();
      break;
    }
  }
}
/*********************************MQTT任务**************************************/
void Mqtt_Task()
{
  if (server_flag) { //如果用户已配置好参数
    if (!client.connected()) {  //服务器掉线重连
      reconnect();
    }
    client.loop();
  }
  if (stringComplete) {
    if (inputString.indexOf("mqtt")) {
      /************************************json 数据处理********************************************/
      String  input =   inputString;
      DynamicJsonBuffer jsonBuffer;
      JsonObject & root = jsonBuffer.parseObject(input);
      String output =  root[String("mqtt")];
      if (output == "wcc") {
        if (!server_flag) {
          CONFIG buf;
          String output =  root[String("server")];
          //"strcpy"是和结构体搭配的语法 。
          //strcpy是一种C语言的标准库函数，strcpy把含有'\0'结束符的字符串复制到另一个地址空间，返回值的类型为char*。
          strcpy(buf.server, output.c_str());//服务器域名
          String output_port =  root[String("port")];
          if (output_port == "") output_port = "1883"; //判断是否启用其他端口
          port = output_port.toInt();
          Serial.println(port);
          String output_id =  root[String("id")];
          mqtt_id = output_id; //设备ID
          String output_user =  root[String("user")];
          mqtt_user = output_user; //设备用户名
          String output_password =  root[String("password")];
          mqtt_password = output_password; //设备密码
          String mqtt_sub_topic =  root[String("sub_topic")];
          if (mqtt_sub_topic != "") sub_topic =  mqtt_sub_topic; //订阅的主题
          else   sub_topic = output; //订阅的主题
          Serial.println("setServer_OK");
          delay(100);
          client.setServer(buf.server, port);
          client.setCallback(callback);
          server_flag = 1;
        }
        String output_topic =  root[String("topic")];
        CONFIG buf;
        if ((output_topic != "") || (buf.onenet_topic_msg != ""))
        {
          CONFIG buf;
          String output_message =  root[String("message")];
          strcpy(buf.topic, output_topic.c_str());
          strcpy(buf.message, output_message.c_str());
          String output_onenet =  root[String("topic&msg")];
          strcpy(buf.onenet_topic_msg, output_onenet.c_str());//onenet专属协议
          //下面进行onenet封包
          if (port == 6002) { //如果连接的是onenet服务器
            snprintf(msgJson, 100, buf.onenet_topic_msg); //数据包最长少于100字节
            json_len = strlen(msgJson); //packet length count the end char '\0'
            msg_buf[0] = char(0x03); //palyLoad packet byte 1, one_net mqtt Publish packet payload byte 1, type3 , json type2
            msg_buf[1] = char(json_len >> 8); //high 8 bits of json_len (16bits as short int type)
            msg_buf[2] = char(json_len & 0xff); //low 8 bits of json_len (16bits as short int type)
            memcpy(msg_buf + 3, msgJson, strlen(msgJson));
            msg_buf[3 + strlen(msgJson)] = 0;
            client.publish("$dp", (uint8_t*)msg_buf, strlen(msgJson) + 3, false);   //入口参数类型和自己的PubSubClient库有关
            memset(msgJson, '\0', sizeof(msgJson));
            memset(msg_buf, '\0', sizeof(msg_buf));
            Serial.print("publish_OK");
            Serial.print("\r\n");
          }
          else {  //如果是其他服务器（百度云服务器待做）
            client.publish(buf.topic, buf.message, true); //数据发送至服务器
            Serial.print("publish_OK");
            Serial.print("\r\n");
          }
        }
      }
    }
    //清除标志位
    inputString = "";
    stringComplete = false;
  }

}
