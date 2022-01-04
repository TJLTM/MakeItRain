#define DaughterBoardSense 2
#define Zone1Input 36 //SVP 
#define Zone1Output 27
#define Zone2Input 39 //SVN
#define Zone2Output 14
#define Zone3Input 34
#define Zone3Output 12
#define Zone4Input 35
#define Zone4Output 23
#define BatteryVoltagePin 4


void setup() {
  Serial.begin(9600);
  pinMode(Zone1Input,INPUT);
  pinMode(Zone2Input,INPUT);
  pinMode(Zone3Input,INPUT);
  pinMode(Zone4Input,INPUT);
  pinMode(DaughterBoardSense,INPUT);
  pinMode(Zone1Output,OUTPUT);
  pinMode(Zone2Output,OUTPUT);
  pinMode(Zone3Output,OUTPUT);
  pinMode(Zone4Output,OUTPUT);
}

void loop() {
  digitalWrite(Zone1Output,digitalRead(Zone1Input));
  digitalWrite(Zone2Output,digitalRead(Zone2Input));
  digitalWrite(Zone3Output,digitalRead(Zone3Input));
  digitalWrite(Zone4Output,digitalRead(Zone4Input));
  Serial.print("DaughterBoardSense:");
  Serial.println(DaughterBoardSense);
  Serial.print("Battery Voltage:");
  Serial.println(analogRead(BatteryVoltagePin));
}
