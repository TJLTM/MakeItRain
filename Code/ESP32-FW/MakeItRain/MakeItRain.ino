#include <PubSubClient.h>
#include <WiFi.h>
#include <Preferences.h>
#include <WiFiClient.h>
#include <WiFiAP.h>

// MQTT client
WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient);

//System Level
String Name = "MakeItRain";
String ID;
int NumberOfWifiReconntionFailures = 0;
int MaxAttempts = 4;
Preferences preferences;
long ThirtyMinTimer,TenSecondTimer, OneSecondTimer;
bool LocalControlLockOut = false;
#define DaughterBoardSense 2
#define BatteryVoltagePin 4
float LastBatteryVoltage;

//Zone definitions
#define Zone1Input 36
#define Zone1Output 27
String ZO1Topic = "";
String ZO1State = "off";
String LastMQTTZO1State = "off";

#define Zone2Input 39
#define Zone2Output 14
String ZO2Topic = "";
String ZO2State = "off";
String LastMQTTZO2State = "off";

#define Zone3Input 34
#define Zone3Output 12
String ZO3Topic = "";
String ZO3State = "off";
String LastMQTTZO3State = "off";

#define Zone4Input 35
#define Zone4Output 23
String ZO4Topic = "";
String ZO4State = "off";
String LastMQTTZO4State = "off";


void setup() {
  Serial.begin(115200);
  Serial.println("Starting to... MAKEITRAIN");
  //SetupAllStoredInformation();

  pinMode(Zone1Input, INPUT);
  attachInterrupt(digitalPinToInterrupt(Zone1Input), LocalInput1, RISING);
  pinMode(Zone1Output, OUTPUT);

  pinMode(Zone2Input, INPUT);
  attachInterrupt(digitalPinToInterrupt(Zone2Input), LocalInput2, RISING);
  pinMode(Zone2Output, OUTPUT);

  pinMode(Zone3Input, INPUT);
  attachInterrupt(digitalPinToInterrupt(Zone3Input), LocalInput3, RISING);
  pinMode(Zone3Output, OUTPUT);

  pinMode(Zone4Input, INPUT);
  attachInterrupt(digitalPinToInterrupt(Zone4Input), LocalInput4, RISING);
  pinMode(Zone4Output, OUTPUT);

  pinMode(DaughterBoardSense, INPUT);
  if (digitalRead(DaughterBoardSense) == HIGH) {
    //other stuff to put here for daughterboard setup
  }

  preferences.begin("SystemSettings", true);
  //setup other System Level settings
  LocalControlLockOut = preferences.getBool("LocalLockOut");
  preferences.end();
  
  String MAC = WiFi.macAddress();
    for (int x = 9; x < 17; x++) {
      if (MAC.charAt(x) != ':') {
        ID.concat(MAC.charAt(x));
      }
    }

  ConnectToDaWEEEEFEEEEEEEE(1, 60000);
  SetupMQTT();

  if (WiFi.status() == WL_CONNECTED) {
    delay(100);
    MqttConnectionCheck();
  }
  else {
    //Turn on Bluetooth and put Wifi into AP mode
  }

  ReadVoltage();
}

void loop() {
  if (WiFi.status() != WL_CONNECTED) {
    ConnectToDaWEEEEFEEEEEEEE(MaxAttempts, 60000);
  }

  if (NumberOfWifiReconntionFailures > MaxAttempts) {
    Serial.println("Connection attempts exhausted");
    LocalControlLockOut = false; //turn off lockout so control via buttons is restored. 
  }

  MqttConnectionCheck();

  /* Enable/Disable the local input Interrupts
      If they are disabled they will be polled and pushed to MQTT for state
  */
  if (LocalControlLockOut == false) {
    interrupts();
  } else {
    noInterrupts();
    long CurrentTime = millis();
    //Read all the inputs and post to MQTT every 10 seconds
    if (abs(OneSecondTimer - CurrentTime) > 1000){
      for (int x = 1; x < 5; x++) {
        String PathName = "/" + Name + "/" + ID + "/ZoneInput/" + String(x);
        mqttClient.publish(PathName.c_str(), String(ReadInput(x)).c_str());
      }
      TenSecondTimer = millis();
    }
  }

  long CurrentTime = millis();
  ReadVoltage();
  if (abs(TenSecondTimer - CurrentTime) > 180000) {
    String VTopic = "/" + Name + "/" + ID + "Votlage";
    mqttClient.publish(VTopic.c_str(), String(LastBatteryVoltage).c_str());
    TenSecondTimer = millis();
  }

 
  if (LastMQTTZO1State != ReadOutput(1)){
    mqttClient.publish(ZO1Topic.c_str(), String(ReadOutput(1)).c_str());
  }

if (LastMQTTZO2State != ReadOutput(2)){
    mqttClient.publish(ZO2Topic.c_str(), String(ReadOutput(2)).c_str());
  }

  if (LastMQTTZO3State != ReadOutput(3)){
    mqttClient.publish(ZO3Topic.c_str(), String(ReadOutput(3)).c_str());
  }

  if (LastMQTTZO4State != ReadOutput(4)){
    mqttClient.publish(ZO4Topic.c_str(), String(ReadOutput(4)).c_str());
  }
  
}

void SetupAllStoredInformation() {
  /*
     Comment out the stuff you don't need to update in perferences.
  */
  preferences.begin("credentials", false);
  //preferences.clear();
  //preferences.putString("ssid", "Your WiFi SSID");
  //preferences.putString("password", "Your Wifi Password");
  preferences.end();

  preferences.begin("SystemSettings", false);
  //preferences.clear();
  //preferences.putBool("LocalLockOut", true);
  //preferences.putString("MQTTIP", "IP"); //Tested with IP not hostnames
  //preferences.putInt("MQTTPORT", 1883);
  preferences.end();
}

//-----------------------------------------------------------------------------------
//Wifi, AP and BLE
//-----------------------------------------------------------------------------------
void ConnectToDaWEEEEFEEEEEEEE(int Attempts, int Timeout) {
  if (NumberOfWifiReconntionFailures < Attempts) {
    preferences.begin("credentials", false);
    Serial.print("Connecting to ");
    Serial.print(preferences.getString("ssid"));
    Serial.print("  Attempt: ");
    Serial.println(NumberOfWifiReconntionFailures);
    WiFi.begin(preferences.getString("ssid").c_str(), preferences.getString("password").c_str());
    preferences.end();
    
    int StartTime = millis();
    int CurrentTime = millis();
    while (WiFi.status() != WL_CONNECTED && abs(StartTime - CurrentTime) < Timeout) {
      delay(500);
      Serial.print(".");
      CurrentTime = millis();
    }
    Serial.println();
    if (WiFi.status() == WL_CONNECTED) {
      NumberOfWifiReconntionFailures = 0;
    }
    else {
      NumberOfWifiReconntionFailures += 1;
    }
  }
}

void SetupAP(){
  //WiFi.softAP(Name);
}

//-----------------------------------------------------------------------------------
//Reading states
//-----------------------------------------------------------------------------------
void ReadVoltage() {
  LastBatteryVoltage = round((30.954 / 4095) * analogRead(BatteryVoltagePin));
}

String ReadOutput(int Number) {
  /*

  */
  String ValueToReturn = "off";
  switch (Number) {
    case 1:
      if (digitalRead(Zone1Output) == 1) {
        ValueToReturn = "on";
      }
      break;
    case 2:
      if (digitalRead(Zone2Output) == 1) {
        ValueToReturn = "on";
      }
      break;
    case 3:
      if (digitalRead(Zone3Output) == 1) {
        ValueToReturn = "on";
      }
      break;
    case 4:
      if (digitalRead(Zone4Output) == 1) {
        ValueToReturn = "on";
      }
      break;
  }

  return ValueToReturn;
}

String ReadInput(int Number) {
  /*

  */
  String ValueToReturn = "off";
  switch (Number) {
    case 1:
      if (digitalRead(Zone1Input) == 1) {
        ValueToReturn = "on";
      }
      break;
    case 2:
      if (digitalRead(Zone2Input) == 1) {
        ValueToReturn = "on";
      }
      break;
    case 3:
      if (digitalRead(Zone3Input) == 1) {
        ValueToReturn = "on";
      }
      break;
    case 4:
      if (digitalRead(Zone4Input) == 1) {
        ValueToReturn = "on";
      }
      break;
  }
  return ValueToReturn;
}


//-----------------------------------------------------------------------------------
//Local control interrupts
//-----------------------------------------------------------------------------------
void LocalInput1() {
  if (digitalRead(Zone1Output) == LOW) {
    SetOutput(1, HIGH);
  }
  else {
    SetOutput(1, LOW);
  }
}

void LocalInput2() {
  if (digitalRead(Zone2Output) == false) {
    SetOutput(2, HIGH);
  }
  else {
    SetOutput(2, LOW);
  }
}

void LocalInput3() {
  if (digitalRead(Zone3Output) == false) {
    SetOutput(3, HIGH);
  }
  else {
    SetOutput(3, LOW);
  }
}

void LocalInput4() {
  if (digitalRead(Zone4Output) == false) {
    SetOutput(4, HIGH);
  }
  else {
    SetOutput(4, LOW);
  }
}

//-----------------------------------------------------------------------------------
//Set Functions 
//-----------------------------------------------------------------------------------
void SetOutput(int Number, bool State) {
  /*

  */
  String Translation = "off";
  if (State == HIGH){
    Translation = "on";
  }
  
  switch (Number) {
    case 1:
      digitalWrite(Zone1Output, State);
      ZO1State = Translation;
      break;
    case 2:
      digitalWrite(Zone2Output, State);
      ZO2State = Translation;
      break;
    case 3:
      digitalWrite(Zone3Output, State);
      ZO3State = Translation;
      break;
    case 4:
      digitalWrite(Zone4Output, State);
      ZO4State = Translation;
      break;
  }
}

//-----------------------------------------------------------------------------------
//MQTT Related 
//-----------------------------------------------------------------------------------
void SetupMQTT(){
    preferences.begin("SystemSettings", true);
    //set up the MQTT
    String TargetFromMem = preferences.getString("MQTTIP");
    char Target[TargetFromMem.length()];
    TargetFromMem.toCharArray(Target,TargetFromMem.length()+1);
    char *mqttServer;
    mqttServer = &Target[0];
    mqttClient.setServer(mqttServer, preferences.getInt("MQTTPORT"));
    mqttClient.setCallback(callback);
    preferences.end();
}

void MqttConnectionCheck() {
  if (WiFi.status() == WL_CONNECTED) {
    if (!mqttClient.connected()) {
      reconnect();
    }
    mqttClient.loop();
  }
}

void reconnect() {
  // Loop until we're reconnected
  while (!mqttClient.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (mqttClient.connect(Name.c_str())) {
      Serial.println("connected");
      // sub to the Zone Output topics and pub the currect state
      ZO1Topic =  "/" + Name + "/" + ID + "/ZoneOutput/1";
      mqttClient.subscribe(ZO1Topic.c_str());
      mqttClient.publish(ZO1Topic.c_str(), String(ReadOutput(1)).c_str());
      ZO2Topic =  "/" + Name + "/" + ID + "/ZoneOutput/2";
      mqttClient.subscribe(ZO2Topic.c_str());
      mqttClient.publish(ZO2Topic.c_str(), String(ReadOutput(2)).c_str());
      ZO3Topic =  "/" + Name + "/" + ID + "/ZoneOutput/3";
      mqttClient.subscribe(ZO3Topic.c_str());
      mqttClient.publish(ZO3Topic.c_str(), String(ReadOutput(3)).c_str());
      ZO4Topic =  "/" + Name + "/" + ID + "/ZoneOutput/4";
      mqttClient.subscribe(ZO4Topic.c_str());
      mqttClient.publish(ZO4Topic.c_str(), String(ReadOutput(4)).c_str());
    }
    else {
      Serial.print("failed, rc=");
      Serial.print(mqttClient.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  String message;
  String CurrentOutputState;
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
    message += (char)payload[i];
  }
  Serial.println();

  //Zone 1
  if (String((char*)topic) == ZO1Topic) {
    CurrentOutputState = String(ReadOutput(1)).c_str();
    LastMQTTZO1State = message;
    if (message != CurrentOutputState) {
      if (message == "on") {
        SetOutput(1, true);
        Serial.println("Turning Zone 1: on");
      }
      if (message == "off") {
        SetOutput(1, false);
        Serial.println("Turning Zone 1: off");
      }
    }
  }

  //Zone 2
  if (String((char*)topic) == ZO2Topic) {
    CurrentOutputState = String(ReadOutput(2)).c_str();
    LastMQTTZO2State = message;
    if (message != CurrentOutputState) {
      if (message == "on") {
        SetOutput(2, true);
        Serial.println("Turning Zone 2: on");
      }
      if (message == "off") {
        SetOutput(2, false);
        Serial.println("Turning Zone 2: off");
      }
    }
  }

  //Zone 3
  if (String((char*)topic) == ZO3Topic) {
    CurrentOutputState = String(ReadOutput(3)).c_str();
    LastMQTTZO3State = message;
    if (message != CurrentOutputState) {
      if (message == "on") {
        SetOutput(3, true);
        Serial.println("Turning Zone 3: on");
      }
      if (message == "off") {
        SetOutput(3, false);
        Serial.println("Turning Zone 3: off");
      }
    }
  }

  //Zone 4
  if (String((char*)topic) == ZO4Topic) {
    CurrentOutputState = String(ReadOutput(4)).c_str();
    LastMQTTZO4State = message;
    if (message != CurrentOutputState) {
      if (message == "on") {
        SetOutput(4, true);
        Serial.println("Turning Zone 4: on");
      }
      if (message == "off") {
        SetOutput(4, false);
        Serial.println("Turning Zone 4: off");
      }
    }
  }

}
