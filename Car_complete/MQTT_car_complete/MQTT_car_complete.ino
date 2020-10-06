#define ENA   14          // Enable/speed motors Right        GPIO14(D5)
#define ENB   12          // Enable/speed motors Left         GPIO12(D6)
#define IN_1  15          // L298N in1 motors Right           GPIO15(D8)
#define IN_2  13          // L298N in2 motors Right           GPIO13(D7)
#define IN_3  2           // L298N in3 motors Left            GPIO2(D4)
#define IN_4  0           // L298N in4 motors Left            GPIO0(D3)

#include <ESP8266WiFi.h>
#include <PubSubClient.h>
WiFiClient espClient;
PubSubClient client(espClient);

int speedCar = 800;         // 400 - 1023.
int speed_Coeff = 3;

const char* wifissid = "TP-LINK_09FD28"; //改成自己家wifi
const char* password = "Liuzilong123"; //改成自己家wifi
const char* mqtt_server = "39.102.32.174";
const char* mqtt_id = "474141956_ESP";   //改成自己的QQ号+_ESP
const char* Mqtt_sub_topic = "474141956_ESP";   //改成自己的QQ号+_ESP
const char* Mqtt_pub_topic = "474141956";  //  上报消息给  手机APP的TOPIC  //改成自己的QQ号
long lastMsg = 0; //定时用的
void setup() {

  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
  pinMode(ENA, OUTPUT);
  pinMode(ENB, OUTPUT);
  pinMode(IN_1, OUTPUT);
  pinMode(IN_2, OUTPUT);
  pinMode(IN_3, OUTPUT);
  pinMode(IN_4, OUTPUT);

}

void setup_wifi() {
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(wifissid);
  WiFi.begin(wifissid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}
void callback(char* topic, byte* payload, unsigned int length) {
  String msg = "";
  String LED_set = "";
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    msg += (char)payload[i];
  }
  Serial.println(msg);
  if (msg.indexOf("led")) //判断包含关键字
  {
    //取出LED_set数据 并执行
    LED_set = msg.substring(msg.indexOf("led\":") + 5, msg.indexOf("}")); //切分出数据

    //根据int值做一个筛选 区分不同的运动模式
    if (LED_set.toInt() == 0) {
      stopRobot();
    }
    else if (LED_set.toInt() == 1) {
      goAhead();
    }
    else if (LED_set.toInt() == 2) {
     goRight();
    }
    else if (LED_set.toInt() == 3) {
      goLeft();
    } 
    else if (LED_set.toInt() == 4) {
      goBack();
    }
    else if (LED_set.toInt() == 5) { 
      goAheadLeft();
    }
    else if (LED_set.toInt() == 6) {
      goBackLeft();
    }
    else if (LED_set.toInt() == 7) {
      goBackRight();
    } 
    else if (LED_set.toInt() == 8) {
      goAheadRight();
    }

  }
}

void goAhead() {

  digitalWrite(IN_1, LOW);
  digitalWrite(IN_2, HIGH);
  analogWrite(ENA, speedCar);

  digitalWrite(IN_3, LOW);
  digitalWrite(IN_4, HIGH);
  analogWrite(ENB, speedCar);
}

void goBack() {

  digitalWrite(IN_1, HIGH);
  digitalWrite(IN_2, LOW);
  analogWrite(ENA, speedCar);

  digitalWrite(IN_3, HIGH);
  digitalWrite(IN_4, LOW);
  analogWrite(ENB, speedCar);
}

void goRight() {

  digitalWrite(IN_1, HIGH);
  digitalWrite(IN_2, LOW);
  analogWrite(ENA, speedCar);

  digitalWrite(IN_3, LOW);
  digitalWrite(IN_4, HIGH);
  analogWrite(ENB, speedCar);
}

void goLeft() {

  digitalWrite(IN_1, LOW);
  digitalWrite(IN_2, HIGH);
  analogWrite(ENA, speedCar);

  digitalWrite(IN_3, HIGH);
  digitalWrite(IN_4, LOW);
  analogWrite(ENB, speedCar);
}

void goAheadRight() {

  digitalWrite(IN_1, HIGH);
  digitalWrite(IN_2, LOW);
  analogWrite(ENA, speedCar / speed_Coeff);

  digitalWrite(IN_3, LOW);
  digitalWrite(IN_4, HIGH);
  analogWrite(ENB, speedCar);
}

void goAheadLeft() {

  digitalWrite(IN_1, LOW);
  digitalWrite(IN_2, HIGH);
  analogWrite(ENA, speedCar);

  digitalWrite(IN_3, HIGH);
  digitalWrite(IN_4, LOW);
  analogWrite(ENB, speedCar / speed_Coeff);
}

void goBackRight() {

  digitalWrite(IN_1, LOW);
  digitalWrite(IN_2, HIGH);
  analogWrite(ENA, speedCar / speed_Coeff);

  digitalWrite(IN_3, HIGH);
  digitalWrite(IN_4, LOW);
  analogWrite(ENB, speedCar);
}

void goBackLeft() {

  digitalWrite(IN_1, HIGH);
  digitalWrite(IN_2, LOW);
  analogWrite(ENA, speedCar);

  digitalWrite(IN_3, LOW);
  digitalWrite(IN_4, HIGH);
  analogWrite(ENB, speedCar / speed_Coeff);
}

void stopRobot() {

  digitalWrite(IN_1, LOW);
  digitalWrite(IN_2, LOW);
  analogWrite(ENA, speedCar);

  digitalWrite(IN_3, LOW);
  digitalWrite(IN_4, LOW);
  analogWrite(ENB, speedCar);
}




void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    if (client.connect(mqtt_id)) {
      Serial.println("connected");
      //连接成功以后就开始订阅
      client.subscribe(Mqtt_sub_topic, 1);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}
void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  long now = millis();
  if (now - lastMsg > 2000) {
    lastMsg = now;
    String json = "{\"temperature\":" + String(analogRead(A0)) + "}";
    client.publish(Mqtt_pub_topic, json.c_str());
  }
}
