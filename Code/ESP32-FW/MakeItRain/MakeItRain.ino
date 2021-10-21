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
  // put your setup code here, to run once:
  pinMode(Zone1Input,INPUT);
  pinMode(Zone2Input,INPUT);
  pinMode(Zone3Input,INPUT);
  pinMode(Zone4Input,INPUT);
  pinMode(DaughterBoardSense,INPUT);
  
  pinMode(Zone1Output,OUTPUT);
  digitalWrite(Zone1Output,Zone1State);
  pinMode(Zone2Output,OUTPUT);
  digitalWrite(Zone2Output,Zone2State);
  pinMode(Zone3Output,OUTPUT);
  digitalWrite(Zone3Output,Zone3State);
  pinMode(Zone4Output,OUTPUT);
  digitalWrite(Zone4Output,Zone4State);
  
}

void loop() {
if (LocalControlLockOut == false){
  ReadLocalControls();
}


}

void ReadLocalControls(){
  /*
   * Read the Local Inputs for the Zones
   */
   
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
      digitalWrite(Zone1Output,State);
      break;
    case 6:
      digitalWrite(Zone1Output,State);
      break;
    case 7:
      digitalWrite(Zone1Output,State);
      break;
    case 8:
      digitalWrite(Zone1Output,State);
      break;
  }
   
  
}
