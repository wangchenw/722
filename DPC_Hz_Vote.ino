#define FILTER_N 50
int Filter_Value;
int sensorValue;
int val; 
#include <Servo.h>

Servo servo; 
int pos = 0;   

void setup() {
  Serial.begin(9600);     
  servo.attach(9);   
}

void loop() {
  Filter_Value = Filter();       
  Serial.print(Filter_Value); 
  val = map(Filter_Value, 260, 500, 0, 179);  
  Serial.print("  <<<<<<<读取电压    舵机角度>>>>>>>     "); 
  Serial.println(val); 
  servo.write(val);  
  delay(10);
}


int Filter() {
  int i;
  int filter_sum = 0;
  for(i = 0; i < FILTER_N; i++) {
    sensorValue = analogRead(A0);
    filter_sum += sensorValue;
    delay(2);
   
  }
  return (int)(filter_sum / FILTER_N); 
  
}
