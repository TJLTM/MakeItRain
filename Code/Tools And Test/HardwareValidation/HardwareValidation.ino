#include <Wire.h>
#include <Adafruit_MCP23X08.h>

#define I2C_SDA_PIN 23
#define I2C_SCL_PIN 22
Adafruit_MCP23X08 GPIOEX;
bool GrootToGo = true;

#define VSVoltagePin 33
#define AUXVoltagePin 32
#define ResetButton 25
#define LEDOut 27

#define Zone1Input 34
#define Zone2Input 35
#define Zone3Input 36
#define Zone4Input 39

void setup() {
  Serial.begin(9600);
  //Wire.begin(I2C_SDA_PIN, I2C_SCL_PIN);
  //Wire.setClock(100000);
  if (!GPIOEX.begin_I2C()) {
    Serial.println("Error can not communicate to MCP23008");
    GrootToGo = false;
  }
  else {
    GPIOEX.pinMode(0, OUTPUT); //Zone 1
    GPIOEX.pinMode(1, OUTPUT); //Zone 2
    GPIOEX.pinMode(2, OUTPUT); //Zone 3
    GPIOEX.pinMode(3, OUTPUT); //Zone 4
    GPIOEX.pinMode(4, OUTPUT); //Zone 5
    GPIOEX.pinMode(5, OUTPUT); //Zone 6
    GPIOEX.pinMode(6, OUTPUT); //Zone 7
    GPIOEX.pinMode(7, OUTPUT); //Zone 8
  }

  pinMode(LEDOut, OUTPUT);
  pinMode(ResetButton, INPUT);
  pinMode(Zone1Input, INPUT);
  pinMode(Zone2Input, INPUT);
  pinMode(Zone3Input, INPUT);
  pinMode(Zone4Input, INPUT);
}

void loop() {
  //ButtonCheck();
  //ReadADCs();
  //ReadInputs();
  OutputWalk();
  //OutputInputcheck();
  //OutputBlink(0,2000);
  //OutputBlink(7, 2000);
}

void OutputInputcheck() {
  SetOutput(0, digitalRead(Zone1Input));
  SetOutput(1, digitalRead(Zone2Input));
  SetOutput(2, digitalRead(Zone3Input));
  SetOutput(3, digitalRead(Zone4Input));
}

void ButtonCheck() {
  int ButtonRead = digitalRead(ResetButton);
  Serial.print("Reset Button State:");
  Serial.println(ButtonRead);
  digitalWrite(LEDOut, digitalRead(ResetButton));
}

void ReadADCs() {
  Serial.print("AUX Voltage:");
  Serial.println(analogRead(AUXVoltagePin));
  Serial.print("VS Voltage:");
  Serial.println(analogRead(VSVoltagePin));
  delay(2500);
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
  for (int x = 0; x < 8; x++) {
    OutputBlink(x, 2000);
  }
}

void OutputBlink(int PortNumber, float Time) {

  String Message = "Zone " + String(PortNumber) + " Output ";
  Serial.println(Message + "High");
  SetOutput(PortNumber, HIGH);
  delay(Time);
  Serial.println(Message + "LOW");
  SetOutput(PortNumber, LOW);
  delay(Time);
}


void SetOutput(int PortNumber, bool State) {
  if (GrootToGo == true) {
    GPIOEX.digitalWrite(PortNumber, State);
  }
  else {
    Serial.println("Can Not communicate with MCP23008");
  }
}
