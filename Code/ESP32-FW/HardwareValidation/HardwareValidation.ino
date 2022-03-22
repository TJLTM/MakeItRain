#define Zone1Input 26
#define Zone1Output 17
#define Zone2Input 27
#define Zone2Output 16
#define Zone3Input 14
#define Zone3Output 15
#define Zone4Input 12
#define Zone4Output 2
#define RTCBatteryVoltagePin 39
#define VSVoltagePin 36
#define ResetButton 22
#define LEDOut 21


void setup() {
  Serial.begin(9600);
  pinMode(Zone1Input, INPUT);
  pinMode(Zone2Input, INPUT);
  pinMode(Zone3Input, INPUT);
  pinMode(Zone4Input, INPUT);
  pinMode(ResetButton, INPUT);
  pinMode(Zone1Output, OUTPUT);
  pinMode(Zone2Output, OUTPUT);
  pinMode(Zone3Output, OUTPUT);
  pinMode(Zone4Output, OUTPUT);
  pinMode(LEDOut, OUTPUT);
}

void loop() {
  //ButtonCheck();
  //ReadADCs();
  //ReadInputs();
  //OutputWalk();
  OutputInputcheck();
}

void OutputInputcheck() {
  digitalWrite(Zone1Output, digitalRead(Zone1Input));
  digitalWrite(Zone2Output, digitalRead(Zone2Input));
  digitalWrite(Zone3Output, digitalRead(Zone3Input));
  digitalWrite(Zone4Output, digitalRead(Zone4Input));
}
void ButtonCheck() {
  digitalWrite(LEDOut, digitalRead(ResetButton));
}
void ReadADCs() {
  Serial.print("RTC Battery Voltage:");
  Serial.println(analogRead(RTCBatteryVoltagePin));
  Serial.print("VS Voltage:");
  Serial.println(analogRead(VSVoltagePin));
}
void ReadInputs() {
  Serial.print("Zone1Input:");
  Serial.println(digitalRead(Zone1Input));
  delay(2000);

  Serial.print("Zone2Input:");
  Serial.println(digitalRead(Zone2Input));
  delay(2000);

  Serial.print("Zone3Input:");
  Serial.println(digitalRead(Zone3Input));
  delay(2000);

  Serial.print("Zone4Input:");
  Serial.println(digitalRead(Zone4Input));
  delay(2000);
}
void OutputWalk() {
  Serial.println("Zone1Output high");
  digitalWrite(Zone1Output, HIGH);
  delay(2000);
  Serial.println("Zone1Output low");
  digitalWrite(Zone1Output, LOW);
  delay(2000);

  Serial.println("Zone2Output high");
  digitalWrite(Zone2Output, HIGH);
  delay(2000);
  Serial.println("Zone2Output low");
  digitalWrite(Zone2Output, LOW);
  delay(2000);

  Serial.println("Zone3Output high");
  digitalWrite(Zone3Output, HIGH);
  delay(2000);
  Serial.println("Zone3Output low");
  digitalWrite(Zone3Output, LOW);
  delay(2000);

  Serial.println("Zone4Output high");
  digitalWrite(Zone4Output, HIGH);
  delay(2000);
  Serial.println("Zone4Output low");
  digitalWrite(Zone4Output, LOW);
  delay(2000);
}
