#include <Wire.h>
#include <Adafruit_MCP23X08.h>
#include <Adafruit_MCP23X17.h>
#include <Adafruit_MCP23XXX.h>

#define VSVoltagePin 33
#define AUXVoltagePin 32
#define ResetButton 25
#define LEDOut 27

#define Zone1Input 34
#define Zone2Input 35
#define Zone3Input 36
#define Zone4Input 39

Adafruit_MCP23X08 GPIOEX;

void setup() {
  Serial.begin(9600);
  pinMode(LEDOut, OUTPUT);
  pinMode(ResetButton, INPUT);
  pinMode(Zone1Input, INPUT);
  pinMode(Zone2Input, INPUT);
  pinMode(Zone3Input, INPUT);
  pinMode(Zone4Input, INPUT);
}

void loop() {
  ButtonCheck();
  ReadADCs();
  ReadInputs();
  OutputWalk();
  OutputInputcheck();
}

void OutputInputcheck() {
  GPIOEX.digitalWrite(0,digitalRead(Zone1Input));
  GPIOEX.digitalWrite(1,digitalRead(Zone2Input));
  GPIOEX.digitalWrite(2,digitalRead(Zone3Input));
  GPIOEX.digitalWrite(3,digitalRead(Zone4Input));
}

void ButtonCheck() {
  digitalWrite(LEDOut, digitalRead(ResetButton));
}

void ReadADCs() {
  Serial.print("AUX Voltage:");
  Serial.println(analogRead(AUXVoltagePin));
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
  for (int x=0; x < 8; x++){
    String Message = "Zone " + String(x+1) + " Output ";
    Serial.println(Message + "High");
    GPIOEX.digitalWrite(x,HIGH);
    delay(2000);
    Serial.println(Message + "LOW");
    GPIOEX.digitalWrite(x,LOW);
    
  }
}
