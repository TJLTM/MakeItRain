#include <RTClib.h>
#include <Wire.h>

#define Zone1Input 26
#define Zone1Output 17
#define Zone2Input 27
#define Zone2Output 16
#define Zone3Input 14
#define Zone3Output 15
#define Zone4Input 12
#define Zone4Output 2 
#define Zone5Output 5
#define Zone6Output 18
#define Zone7Output 19
#define Zone8Output 23
#define RTCBatteryVoltagePin 39
#define VSVoltagePin 36
#define ResetButton 22
#define LEDOut 21

RTC_DS3231 rtc;
char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};

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
  pinMode(Zone5Output, OUTPUT);
  pinMode(Zone6Output, OUTPUT);
  pinMode(Zone7Output, OUTPUT);
  pinMode(Zone8Output, OUTPUT);
  pinMode(LEDOut, OUTPUT);
  rtc.begin();
}

void loop() {
  //ButtonCheck();
  //ReadADCs();
  //ReadInputs();
  //OutputWalk();
  //OutputInputcheck();
  //EXTOutputWalk();
  RTCCheck();
}

void RTCCheck() {
  DateTime now = rtc.now();

  Serial.print(now.year(), DEC);
  Serial.print('/');
  Serial.print(now.month(), DEC);
  Serial.print('/');
  Serial.print(now.day(), DEC);
  Serial.print(" (");
  Serial.print(daysOfTheWeek[now.dayOfTheWeek()]);
  Serial.print(") ");
  Serial.print(now.hour(), DEC);
  Serial.print(':');
  Serial.print(now.minute(), DEC);
  Serial.print(':');
  Serial.print(now.second(), DEC);
  Serial.println();

  Serial.print(" since midnight 1/1/1970 = ");
  Serial.print(now.unixtime());
  Serial.print("s = ");
  Serial.print(now.unixtime() / 86400L);
  Serial.println("d");

  // calculate a date which is 7 days, 12 hours, 30 minutes, 6 seconds into the future
  DateTime future (now + TimeSpan(7, 12, 30, 6));

  Serial.print(" now + 7d + 12h + 30m + 6s: ");
  Serial.print(future.year(), DEC);
  Serial.print('/');
  Serial.print(future.month(), DEC);
  Serial.print('/');
  Serial.print(future.day(), DEC);
  Serial.print(' ');
  Serial.print(future.hour(), DEC);
  Serial.print(':');
  Serial.print(future.minute(), DEC);
  Serial.print(':');
  Serial.print(future.second(), DEC);
  Serial.println();

  Serial.print("Temperature: ");
  Serial.print(rtc.getTemperature());
  Serial.println(" C");

  Serial.println();
  delay(3000);
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

void EXTOutputWalk() {
  Serial.println("Zone5Output high");
  digitalWrite(Zone5Output, HIGH);
  delay(2000);
  Serial.println("Zone5Output low");
  digitalWrite(Zone5Output, LOW);
  delay(2000);

  Serial.println("Zone6Output high");
  digitalWrite(Zone6Output, HIGH);
  delay(2000);
  Serial.println("Zone6Output low");
  digitalWrite(Zone6Output, LOW);
  delay(2000);

  Serial.println("Zone7Output high");
  digitalWrite(Zone7Output, HIGH);
  delay(2000);
  Serial.println("Zone7Output low");
  digitalWrite(Zone7Output, LOW);
  delay(2000);

  Serial.println("Zone8Output high");
  digitalWrite(Zone8Output, HIGH);
  delay(2000);
  Serial.println("Zone8Output low");
  digitalWrite(Zone8Output, LOW);
  delay(2000);
}
