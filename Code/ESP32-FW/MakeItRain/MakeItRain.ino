//#include <PubSubClient.h>
#include <Wire.h>
#include <WiFi.h>
#include <HTTP_Method.h>
#include <Uri.h>
#include <WebServer.h>
#include <Preferences.h>

const char* ssid     = "your-ssid";
const char* password = "your-password";

bool LocalControlLockOut = false;
#define DaughterBoardSense 2
#define BatteryVoltagePin 4
float LastBatteryVoltage = 0.0;

#define Zone1Input 36 //SVP 
#define Zone1Output 27

#define Zone2Input 39 //SVN
#define Zone2Output 14

#define Zone3Input 34
#define Zone3Output 12

#define Zone4Input 35
#define Zone4Output 23

#define Zone5Input 36
#define Zone5Output 27

#define Zone6Input 39
#define Zone6Output 14


void setup() {
  Serial.begin(115200);
  
  pinMode(Zone1Input,INPUT);
  attachInterrupt(digitalPinToInterrupt(Zone1Input),LocalInput1,RISING);
  pinMode(Zone1Output,OUTPUT);
  
  pinMode(Zone2Input,INPUT);
  attachInterrupt(digitalPinToInterrupt(Zone2Input),LocalInput2,RISING);
  pinMode(Zone2Output,OUTPUT);
  
  pinMode(Zone3Input,INPUT);
  attachInterrupt(digitalPinToInterrupt(Zone3Input),LocalInput3,RISING);
  pinMode(Zone3Output,OUTPUT);
  
  pinMode(Zone4Input,INPUT);
  attachInterrupt(digitalPinToInterrupt(Zone4Input),LocalInput4,RISING);
  pinMode(Zone4Output,OUTPUT);
  
  pinMode(DaughterBoardSense,INPUT);
  if (digitalRead(DaughterBoardSense) == HIGH){
    //put the stuff for outputs 5-8 in here when you get around to it.
    //Future Idea if using I2C for something maybe check to see if there is something there  
    pinMode(Zone5Input,INPUT);
    attachInterrupt(digitalPinToInterrupt(Zone5Input),LocalInput5,RISING);
    pinMode(Zone5Output,OUTPUT);
    
    pinMode(Zone6Input,INPUT);
    attachInterrupt(digitalPinToInterrupt(Zone6Input),LocalInput6,RISING);
    pinMode(Zone6Output,OUTPUT);
    }

//Setup Wifi Connection 
  //Serial.print("Connecting to ");
  //Serial.println(ssid);
//  while (WiFi.status() != WL_CONNECTED) {
//        delay(500);
//        Serial.print(".");
//    }
}

void loop() {
if (LocalControlLockOut == false){interrupts();}else {noInterrupts();}
ReadVoltage();


}

void ReadVoltage(){
  int DN = analogRead(BatteryVoltagePin);
  LastBatteryVoltage = DN;
  Serial.print("DN for battery:");
  Serial.println(LastBatteryVoltage);
}

void LocalInput1(){
  if (digitalRead(Zone1Output) == LOW){
    SetOutput(1,HIGH);
  }
  else{
    SetOutput(1,LOW);
  }
}

void LocalInput2(){
  if (digitalRead(Zone2Output) == false){
    SetOutput(2,HIGH);
  }
  else{
    SetOutput(2,LOW);
  }
}

void LocalInput3(){
  if (digitalRead(Zone3Output) == false){
    SetOutput(3,HIGH);
  }
  else{
    SetOutput(3,LOW);
  }
}

void LocalInput4(){
  if (digitalRead(Zone4Output) == false){
    SetOutput(4,HIGH);
  }
  else{
    SetOutput(4,LOW);
  }
}

void LocalInput5(){
  if (digitalRead(Zone5Output) == LOW){
    SetOutput(5,HIGH);
  }
  else{
    SetOutput(5,LOW);
  }
}

void LocalInput6(){
  if (digitalRead(Zone6Output) == false){
    SetOutput(6,HIGH);
  }
  else{
    SetOutput(6,LOW);
  }
}

void SetOutput(int Number,bool State){
  /*
   * 
   */
  switch(Number){
    case 1:
      digitalWrite(Zone1Output,State);
      break;
    case 2:
      digitalWrite(Zone2Output,State);
      break;
    case 3:
      digitalWrite(Zone3Output,State);
      break;
    case 4:
      digitalWrite(Zone4Output,State);
      break;
    case 5:
      digitalWrite(Zone5Output,State);
      break;
    case 6:
      digitalWrite(Zone6Output,State);
      break;
  }
   
  
}
