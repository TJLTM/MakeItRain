#include <PubSubClient.h>
#include <WiFi.h>
#include <Preferences.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include <ESPmDNS.h>
#include <Update.h>
#include <WiFiAP.h>

// Needed webserver files
#include "ESPAsyncWebServer.h"
#include "SPIFFS.h"

//Create server on port 80
AsyncWebServer server(80);

// MQTT client
WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient);

//System Level
bool debug = true;
String Name = "MakeItRain";
String ID;
int NumberOfWifiReconntionFailures = 0;
int MaxAttempts = 4;
Preferences preferences;
long ThirtyMinTimer, ThirtySecondTimer, TenSecondTimer, FifthteenSecondTimer, WifiTryAgainTimer, FiveSecondTimer;
bool LocalControlLockOut = false;
#define RTCBatteryVoltagePin 39
#define VSVoltagePin 36
#define ResetButton 22
#define LEDOut 21
float LastVSVoltage, LastRTCBatteryVoltage;

//Zone definitions
#define Zone1Input 26
#define Zone1Output 17
String ZO1Topic = "";
String LastMQTTZO1State = "off";
String LastZIN1State;
float ZO1MaxOn;
long Zone1TurnedOnTime;

#define Zone2Input 27
#define Zone2Output 16
String ZO2Topic = "";
String LastMQTTZO2State = "off";
String LastZIN2State;
float ZO2MaxOn;
long Zone2TurnedOnTime;

#define Zone3Input 14
#define Zone3Output 15
String ZO3Topic = "";
String LastMQTTZO3State = "off";
String LastZIN3State;
float ZO3MaxOn;
long Zone3TurnedOnTime;

#define Zone4Input 12
#define Zone4Output 2
String ZO4Topic = "";
String LastMQTTZO4State = "off";
String LastZIN4State;
float ZO4MaxOn;
long Zone4TurnedOnTime;

String processor(const String& var){
  // Place holder
  Serial.println(var);
  return String();
}

void setup() {
  Serial.begin(115200);
  SerialOutput("Starting to... MAKEITRAIN", true);
  //SetupAllStoredInformation();

  if(!SPIFFS.begin(true)){
    Serial.println("An Error has occured while mounting SPIFFS");
    return;
  }

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

  preferences.begin("SystemSettings", true);
  //setup other System Level settings
  LocalControlLockOut = preferences.getBool("LocalLockOut");

  ZO1MaxOn = preferences.getFloat("Z1_Max");
  ZO2MaxOn = preferences.getFloat("Z2_Max");
  ZO3MaxOn = preferences.getFloat("Z3_Max");
  ZO4MaxOn = preferences.getFloat("Z4_Max");

  preferences.end();

  String MAC = WiFi.macAddress();
  for (int x = 9; x < 17; x++) {
    if (MAC.charAt(x) != ':') {
      ID.concat(MAC.charAt(x));
    }
  }

  ConnectToDaWEEEEFEEEEEEEE(1, 60000);
  SetupMQTT();
  webserverAPI();

  if (WiFi.status() == WL_CONNECTED) {
    delay(100);
    MqttConnectionCheck();
  }
  else {
    //Turn on Bluetooth and put Wifi into AP mode
  }

  SetOutput(1, false);
  SetOutput(2, false);
  SetOutput(3, false);
  SetOutput(4, false);
  ReadVoltage();
}

void loop() {
  long CurrentTime = millis();

  if (WiFi.status() != WL_CONNECTED) {
    ConnectToDaWEEEEFEEEEEEEE(MaxAttempts, 60000);
  }

  if (NumberOfWifiReconntionFailures > MaxAttempts && abs(FiveSecondTimer - CurrentTime) > 5000) {
    SerialOutput("Connection attempts exhausted", true);
    LocalControlLockOut = false; //turn off lockout so control via buttons is restored.
    FiveSecondTimer = millis();
    if (abs(WifiTryAgainTimer - CurrentTime) > 900000) {
      NumberOfWifiReconntionFailures = 0;
      WifiTryAgainTimer = millis();
    }
  }

  MqttConnectionCheck();

  /* Enable/Disable the local input Interrupts
      If they are disabled they will be polled and pushed to MQTT for state
  */
  if (LocalControlLockOut == false) {
    interrupts();
  } else {
    noInterrupts();
    //Read all the inputs and post if changed from last read.
    CheckIfInputsHaveChanged();
  }

//  if (abs(FifthteenSecondTimer - CurrentTime) > 15000) {
//
//    FifthteenSecondTimer = millis();
//  }

  if (abs(ThirtySecondTimer - CurrentTime) > 30000) {
    ReadVoltage();
    ThirtySecondTimer = millis();
  }

  MaxZoneTimeOnCheck();

}

void SetupAllStoredInformation() {
  /*
     Comment out the stuff you don't need to update in perferences.
  */
  preferences.begin("credentials", false);
  preferences.clear();
  preferences.putString("ssid", "HIOT");
  preferences.putString("password", "flyingFalcon83!");
  preferences.putString("Admin_password", "SoOriginalThereBoss");
  preferences.end();

  preferences.begin("SystemSettings", false);
  preferences.clear();
  preferences.putBool("LocalLockOut", true);
  preferences.putString("MQTTIP", "10.10.0.2"); //Tested with IP not hostnames
  preferences.putInt("MQTTPORT", 1883);

  preferences.putFloat("Z1_Max", 7.5);
  preferences.putFloat("Z2_Max", 7.5);
  preferences.putFloat("Z3_Max", 7.5);
  preferences.putFloat("Z4_Max", 7.5);
  preferences.end();
}

//-----------------------------------------------------------------------------------
//Wifi, AP and BLE
//-----------------------------------------------------------------------------------
void ConnectToDaWEEEEFEEEEEEEE(int Attempts, int Timeout) {
  if (NumberOfWifiReconntionFailures < Attempts) {
    preferences.begin("credentials", false);
    SerialOutput("Connecting to ", false);
    SerialOutput(preferences.getString("ssid"), false);
    SerialOutput("  Attempt: ", false);
    SerialOutput(String(NumberOfWifiReconntionFailures), true);
    WiFi.begin(preferences.getString("ssid").c_str(), preferences.getString("password").c_str());
    preferences.end();

    int StartTime = millis();
    int CurrentTime = millis();
    while (WiFi.status() != WL_CONNECTED && abs(StartTime - CurrentTime) < Timeout) {
      delay(500);
      SerialOutput(".", false);
      CurrentTime = millis();
    }
    SerialOutput("", true);
    if (WiFi.status() == WL_CONNECTED) {
      NumberOfWifiReconntionFailures = 0;
    }
    else {
      NumberOfWifiReconntionFailures += 1;
    }
  }
}

void SetupAP() {
  //WiFi.softAP(Name);
}

//-----------------------------------------------------------------------------------
//Reading states
//-----------------------------------------------------------------------------------
void ReadVoltage() {
  LastVSVoltage = (30.954 / 4095) * analogRead(VSVoltagePin);
  SerialOutput("Voltage:" + String(LastVSVoltage), true);
  String VTopic = "/" + Name + "/" + ID + "/Voltage";
  mqttClient.publish(VTopic.c_str(), String(LastVSVoltage).c_str());

  LastRTCBatteryVoltage = (30.954 / 4095) * analogRead(RTCBatteryVoltagePin);
  SerialOutput("RTC Voltage:" + String(LastRTCBatteryVoltage), true);
  VTopic = "/" + Name + "/" + ID + "/RTC Battery Votlage";
  mqttClient.publish(VTopic.c_str(), String(LastRTCBatteryVoltage).c_str());
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

void CheckIfInputsHaveChanged() {

  String PathName = "/" + Name + "/" + ID + "/ZoneInput/";
  if (LastZIN1State != ReadInput(1)) {
    PathName = PathName + "1";
    LastZIN1State = String(ReadInput(1));
    mqttClient.publish(PathName.c_str(), LastZIN1State.c_str());
    SerialOutput("Input:1:" + ReadInput(1), true);

  }

  if (LastZIN2State != ReadInput(2)) {
    PathName = PathName + "2";
    LastZIN2State = String(ReadInput(2));
    mqttClient.publish(PathName.c_str(), LastZIN2State.c_str());
    SerialOutput("Input:2:" + ReadInput(2), true);
  }

  if (LastZIN3State != ReadInput(3)) {
    PathName = PathName + "3";
    LastZIN3State = String(ReadInput(3));
    mqttClient.publish(PathName.c_str(), LastZIN3State.c_str());
    SerialOutput("Input:3:" + ReadInput(3), true);
  }

  if (LastZIN4State != ReadInput(4)) {
    PathName = PathName + "4";
    LastZIN4State = String(ReadInput(4));
    mqttClient.publish(PathName.c_str(), LastZIN4State.c_str());
    SerialOutput("Input:4:" + ReadInput(4), true);
  }


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
  if (digitalRead(Zone2Output) == LOW) {
    SetOutput(2, HIGH);
  }
  else {
    SetOutput(2, LOW);
  }
}

void LocalInput3() {
  if (digitalRead(Zone3Output) == LOW) {
    SetOutput(3, HIGH);
  }
  else {
    SetOutput(3, LOW);
  }
}

void LocalInput4() {
  if (digitalRead(Zone4Output) == LOW) {
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
  switch (Number) {
    case 1:
      digitalWrite(Zone1Output, State);
      if (State == HIGH) {
        Zone1TurnedOnTime = millis();
      }

      if (LastMQTTZO1State != ReadOutput(1)) {
        mqttClient.publish(ZO1Topic.c_str(), String(ReadOutput(1)).c_str());
      }
      break;
    case 2:
      digitalWrite(Zone2Output, State);
      if (State == HIGH) {
        Zone2TurnedOnTime = millis();
      }
      if (LastMQTTZO2State != ReadOutput(2)) {
        mqttClient.publish(ZO2Topic.c_str(), String(ReadOutput(2)).c_str());
      }
      break;
    case 3:
      digitalWrite(Zone3Output, State);
      if (State == HIGH) {
        Zone3TurnedOnTime = millis();
      }
      if (LastMQTTZO3State != ReadOutput(3)) {
        mqttClient.publish(ZO3Topic.c_str(), String(ReadOutput(3)).c_str());
      }
      break;
    case 4:
      digitalWrite(Zone4Output, State);
      if (State == HIGH) {
        Zone4TurnedOnTime = millis();
      }

      if (LastMQTTZO4State != ReadOutput(4)) {
        mqttClient.publish(ZO4Topic.c_str(), String(ReadOutput(4)).c_str());
      }
      break;
  }
}

//-----------------------------------------------------------------------------------
//Timer Related
//-----------------------------------------------------------------------------------

void MaxZoneTimeOnCheck() {
  long CurrentTime = millis();
  if (digitalRead(Zone1Output) == HIGH && abs(CurrentTime - Zone1TurnedOnTime) >= ZO1MaxOn * 60000) {
    SetOutput(1, LOW);
  }

  if (digitalRead(Zone2Output) == HIGH && abs(CurrentTime - Zone2TurnedOnTime) >= ZO2MaxOn * 60000) {
    SetOutput(2, LOW);
  }

  if (digitalRead(Zone3Output) == HIGH && abs(CurrentTime - Zone3TurnedOnTime) >= ZO3MaxOn * 60000) {
    SetOutput(3, LOW);
  }

  if (digitalRead(Zone4Output) == HIGH && abs(CurrentTime - Zone4TurnedOnTime) >= ZO4MaxOn * 60000) {
    SetOutput(4, LOW);
  }
}

//-----------------------------------------------------------------------------------
//Communication (MQTT/Serial/WEB)
//-----------------------------------------------------------------------------------
void SerialOutput(String Data, bool CR) {
  if (CR == true) {
    Serial.println(Data);
  }
  else {
    Serial.print(Data);
  }
}

void SetupMQTT() {
  preferences.begin("SystemSettings", true);
  //set up the MQTT
  String TargetFromMem = preferences.getString("MQTTIP");
  char Target[TargetFromMem.length()];
  TargetFromMem.toCharArray(Target, TargetFromMem.length() + 1);
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
  else {
    SerialOutput("Wifi not connected", true);
  }
}

void reconnect() {
  // Loop until we're reconnected
  int MQttReconnect = 0;
  while (!mqttClient.connected() && MQttReconnect < 5) {
    SerialOutput("Attempting MQTT connection, Attempt:" + String(MQttReconnect) + "...", false);
    // Attempt to connect
    mqttClient.disconnect();
    SetupMQTT();

    if (mqttClient.connect(ID.c_str())) {
      SerialOutput("connected", true);
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
      SerialOutput("failed, rc=", false);
      SerialOutput(String(mqttClient.state()), false);
      SerialOutput(" try again in 5 seconds", true);
      // Wait 5 seconds before retrying
      SerialOutput(String(WiFi.status()), true);
      delay(5000);
    }
    MQttReconnect += 1;
  }
}

void callback(char* topic, byte* payload, unsigned int length) {
  SerialOutput("Message arrived [", false);
  SerialOutput(topic, false);
  SerialOutput("] ", false);
  String message;
  String CurrentOutputState;
  for (int i = 0; i < length; i++) {
    message += (char)payload[i];
  }
  SerialOutput(String(message), true);


  //Zone 1
  if (String((char*)topic) == ZO1Topic) {
    CurrentOutputState = String(ReadOutput(1)).c_str();
    LastMQTTZO1State = message;
    if (message != CurrentOutputState) {
      if (message == "on") {
        SetOutput(1, true);
        SerialOutput("Turning Zone 1: on", true);
      }
      if (message == "off") {
        SetOutput(1, false);
        SerialOutput("Turning Zone 1: off", true);
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
        SerialOutput("Turning Zone 2: on", true);
      }
      if (message == "off") {
        SetOutput(2, false);
        SerialOutput("Turning Zone 2: off", true);
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
        SerialOutput("Turning Zone 3: on", true);
      }
      if (message == "off") {
        SetOutput(3, false);
        SerialOutput("Turning Zone 3: off", true);
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
        SerialOutput("Turning Zone 4: on", true);
      }
      if (message == "off") {
        SetOutput(4, false);
        SerialOutput("Turning Zone 4: off", true);
      }
    }
  }

}

void webserverAPI(){

  // Route for root / web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/index.html", String(), false, processor);
  });

  server.on("/styles.css", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/styles.css", "text/css");
  });

  server.on("/main.js", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/main.js", "text/javascript");
  });

  server.on("/polyfills.js", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/polyfills.js", "text/javascript");
  });

  server.on("/runtime.js", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/runtime.js", "text/javascript");
  });

  server.on("/favicon.ico", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/favicon.ico", "image/x-icon");
  });

  server.begin()

}