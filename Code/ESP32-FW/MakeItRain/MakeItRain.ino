#include <PubSubClient.h>
#include <Wire.h>
#include <WiFi.h>
#include <HTTP_Method.h>
#include <Uri.h>
#include <WebServer.h>


bool LocalControlLockOut = false;
#define DaughterBoardSense 2

#define Zone1Input 36 //SVP 
#define Zone1Output 27
bool Zone1State = false;

#define Zone2Input 39 //SVN
#define Zone2Output 14
bool Zone2State = false;

#define Zone3Input 34
#define Zone3Output 12
bool Zone3State = false;

#define Zone4Input 35
#define Zone4Output 23
bool Zone4State = false;


void setup() {
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
  }
  
}

void loop() {
if (LocalControlLockOut == false){interrupts();}else {noInterrupts();}
ReadVoltage();


}

void ReadVoltage(){
  
}

void LocalInput1(){
  if (Zone1State == false){
    Zone1State = true;
    SetOutput(1,HIGH);
  }
  else{
    Zone1State = false;
    SetOutput(1,LOW);
  }
}

void LocalInput2(){
  if (Zone2State == false){
    Zone2State = true;
    SetOutput(2,HIGH);
  }
  else{
    Zone2State = false;
    SetOutput(2,LOW);
  }
}

void LocalInput3(){
  if (Zone3State == false){
    Zone3State = true;
    SetOutput(3,HIGH);
  }
  else{
    Zone3State = false;
    SetOutput(3,LOW);
  }
}

void LocalInput4(){
  if (Zone4Input == false){
    Zone4State = true;
    SetOutput(4,HIGH);
  }
  else{
    Zone4State = false;
    SetOutput(4,LOW);
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
//    case 5:
//      digitalWrite(Zone1Output,State);
//      break;
//    case 6:
//      digitalWrite(Zone1Output,State);
//      break;
//    case 7:
//      digitalWrite(Zone1Output,State);
//      break;
//    case 8:
//      digitalWrite(Zone1Output,State);
//      break;
  }
   
  
}
