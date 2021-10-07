#include <WiFi.h>
#include <HTTP_Method.h>
#include <Uri.h>
#include <WebServer.h>

bool LocalControlLockOut = false;

#define Zone1Input 0
#define Zone1Output 0
bool Zone1State = false;

#define Zone2Input 0
#define Zone2Output 0
bool Zone2State = false;

#define Zone3Input 0
#define Zone3Output 0
bool Zone3State = false;

#define Zone4Input 0
#define Zone4Output 0
bool Zone4State = false;


void setup() {
  // put your setup code here, to run once:
  pinMode(Zone1Input,INPUT);
  pinMode(Zone2Input,INPUT);
  pinMode(Zone3Input,INPUT);
  pinMode(Zone4Input,INPUT);
  
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

}

void ReadLocalControls(){
  /*
   * Read the Local Inputs for the Zones
   */
}

void SetOutput(int Number){
  /*
   * 
   */
  
}
