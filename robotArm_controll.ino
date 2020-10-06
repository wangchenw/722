#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <Arduino_JSON.h>
#include <Servo.h>
WiFiClient espClient;
PubSubClient client(espClient);
Servo myservo1;
Servo myservo2;
Servo myservo3;
Servo myservo4;
Servo myservo5;
Servo myservo6;

int a1,a2,a3,a4,a5,a6;

const char* wifissid = "TP-LINK_09FD28"; //改成自己家wifi
const char* password = "Liuzilong123"; //改成自己家wifi
const char* mqtt_server = "39.97.245.18";
const char* mqtt_id = "robotarm_8266";   //改成自己的QQ号+_ESP
const char* Mqtt_sub_topic = "topic";   //改成自己的QQ号+_ESP
const char* Mqtt_pub_topic = "from_robot_arm";  //  上报消息给  手机APP的TOPIC  //改成自己的QQ号
long lastMsg = 0; //定时用的
void setup() {

  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
  myservo1.attach(16);
  myservo2.attach(5);
  myservo3.attach(4);
  myservo4.attach(0);
  myservo5.attach(2);
  myservo6.attach(14);

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
  Serial.println("] ");
  for (int i = 0; i < length; i++) {
    msg += (char)payload[i];
  }
  //Serial.println(msg);
 

  JSONVar myObject = JSON.parse(msg);
  // JSON.typeof(jsonVar) can be used to get the type of the var
  if (JSON.typeof(myObject) == "undefined") {
    Serial.println("Parsing input failed!");
    return;
  }

 

 

  if (myObject.hasOwnProperty("angle1")) {
    Serial.print("myObject[\"angle1\"] = ");
    a1=(int) myObject["angle1"]; 
    Serial.println(a1); 
    a1 = map(a1, 0, 1023, 0, 180);
    myservo1.write(a1);     
  }

    
    if (myObject.hasOwnProperty("angle2")) {
    Serial.print("myObject[\"angle2\"] = ");
    a2=(int) myObject["angle2"]; 
    Serial.println(a2);
    a2 = map(a2, 0, 1023, 0, 180);
    myservo2.write(a2);    
  }
  
    if (myObject.hasOwnProperty("angle3")) {
    Serial.print("myObject[\"angle3\"] = ");
    a3=(int) myObject["angle3"]; 
    Serial.println(a3);
    a3 = map(a3, 0, 1023, 0, 180);
    myservo3.write(a3);       
  }
   if (myObject.hasOwnProperty("angle4")) {
    Serial.print("myObject[\"angle4\"] = ");
    a4=(int) myObject["angle4"]; 
    Serial.println(a4);
    a4 = map(a4, 0, 1023, 0, 180);
    myservo4.write(a4);   
  }

    
    if (myObject.hasOwnProperty("angle5")) {
    Serial.print("myObject[\"angle5\"] = ");
    a5=(int) myObject["angle5"]; 
    Serial.println(a5);
    a5 = map(a5, 0, 1023, 0, 180);
    myservo5.write(a5);  
  }
  
    if (myObject.hasOwnProperty("angle6")) {
    Serial.print("myObject[\"angle6\"] = ");
    a6=(int) myObject["angle6"]; 
    Serial.println(a6);
    a6 = map(a6, 0, 1023, 0, 180);
    myservo6.write(a6);
  } 
 
  Serial.println();
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
