#include <Arduino_JSON.h>
String jsonString,StringAngle;
int ang1,ang2,ang3,ang4,ang5,ang6;
//创造一个json变量
JSONVar myObject;
JSONVar angles;
void setup() {
  Serial.begin(115200);
}
void loop()
{
  demoCreation();
  delay(50);
}
void demoCreation() {
  ang1 = analogRead(A0);
  ang2=ang1+random(-100,100);
  ang3=ang1+random(-100,100);
  ang4=ang1+random(-100,100);
  ang5=ang1+random(-100,100);
  ang6=ang1+random(-100,100);
  angles["angle1"] = ang1;
  angles["angle2"] = ang2;
  angles["angle3"] = ang3;
  angles["angle4"] = ang4;
  angles["angle5"] = ang5;
  angles["angle6"] = ang6;
  StringAngle = JSON.stringify(angles);
  //给json变量赋值
  myObject["wz"] = "wenzheng.club";
  myObject["server"] = "39.97.245.18";
  myObject["user"] = "root";
  myObject["password"] = "Wangchen222";
  myObject["topic"] = "topic";
  myObject["message"] = StringAngle;
 
  //将json转化为字符串类型
  jsonString = JSON.stringify(myObject);
  Serial.println(jsonString);
}
