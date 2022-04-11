#include <PubSubClient.h>
#include <WiFi.h>
#include <Preferences.h>
#include <WiFiClient.h>
#include <Update.h>
#include <WiFiAP.h>
#include <nvs_flash.h>

//NTP
#include "time.h"
//Internal RTC
#include <ESP32Time.h>

// Needed webserver files
#include "ESPAsyncWebServer.h"
#include "SPIFFS.h"

//Create server on port 80
AsyncWebServer server(80);

// MQTT client
WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient);

//System Level
bool OldHW = false;
String Version = "0.0.1";
bool EnableMQTT, APMode, EnableWifi, Battery;
String Name = "MakeItRain";
String ID;
int NumberOfWifiReconntionFailures = 0;
int MaxAttempts = 4;
int WifiReattemptsBeforeAP = 0;
Preferences preferences;
long ThirtyMinTimer, VoltageTimer, TenSecondTimer, FifthteenSecondTimer, WifiTryAgainTimer, FiveSecondTimer;
bool LocalControlLockOut = false;
#define RTCBatteryVoltagePin 39
#define VSVoltagePin 36
#define ResetButton 22
#define LEDOut 21
float LastVSVoltage;

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


void setup() {
  Serial.begin(115200);
  SerialOutput("Starting to... MAKEITRAIN  Version:" + Version, true);
  CheckStoredData();

  if (OldHW == false) {
#define Zone1Input 26
#define Zone1Output 17
#define Zone2Input 27
#define Zone2Output 16
#define Zone3Input 14
#define Zone3Output 15
#define Zone4Input 12
#define Zone4Output 2
#define VSVoltagePin 36
  }

  pinMode(Zone1Input, INPUT);
  attachInterrupt(digitalPinToInterrupt(Zone1Input), LocalInput1, FALLING);
  pinMode(Zone1Output, OUTPUT);

  pinMode(Zone2Input, INPUT);
  attachInterrupt(digitalPinToInterrupt(Zone2Input), LocalInput2, FALLING);
  pinMode(Zone2Output, OUTPUT);

  pinMode(Zone3Input, INPUT);
  attachInterrupt(digitalPinToInterrupt(Zone3Input), LocalInput3, FALLING);
  pinMode(Zone3Output, OUTPUT);

  pinMode(Zone4Input, INPUT);
  attachInterrupt(digitalPinToInterrupt(Zone4Input), LocalInput4, FALLING);
  pinMode(Zone4Output, OUTPUT);

  preferences.begin("SystemSettings", true);
  //setup other System Level settings
  LocalControlLockOut = preferences.getBool("LocalLockOut");

  ZO1MaxOn = preferences.getFloat("Z1_Max");
  ZO2MaxOn = preferences.getFloat("Z2_Max");
  ZO3MaxOn = preferences.getFloat("Z3_Max");
  ZO4MaxOn = preferences.getFloat("Z4_Max");

  EnableWifi = preferences.getBool("EnableWIFI");
  EnableMQTT = preferences.getBool("EnableMQTT");
  APMode = preferences.getBool("APMode");
  Battery = preferences.getBool("Battery");

  preferences.end();

  String MAC = WiFi.macAddress();
  for (int x = 9; x < 17; x++) {
    if (MAC.charAt(x) != ':') {
      ID.concat(MAC.charAt(x));
    }
  }

  if (EnableWifi == false) {
    ConnectToDaWEEEEFEEEEEEEE(1, 60000);
    if (EnableMQTT == true) {
      SetupMQTT();
    }
  }

  if (WiFi.status() == WL_CONNECTED && EnableWifi == true && EnableMQTT == true) {
    delay(100);
    MqttConnectionCheck();
  }

  if (APMode == true) {
    //Turn on Bluetooth and put Wifi into AP mode
    SetupAP();
  }

  SetOutput(1, false);
  SetOutput(2, false);
  SetOutput(3, false);
  SetOutput(4, false);
  ReadVoltage();

  if (!SPIFFS.begin(true)) {
    Serial.println("An Error has occured while mounting SPIFFS. Can not start WebServer");
  }
  else {
    webserverAPI();
    server.begin();
  }

}

void loop() {
  long CurrentTime = millis();

  if (WiFi.status() != WL_CONNECTED && EnableWifi == true) {
    if (WifiReattemptsBeforeAP < 4) {
      ConnectToDaWEEEEFEEEEEEEE(MaxAttempts, 60000);
    }

    if (abs(WifiTryAgainTimer - CurrentTime) > 900000) {
      NumberOfWifiReconntionFailures = 0;
      WifiTryAgainTimer = millis();
      WifiReattemptsBeforeAP += 1;
    }
  }

  if (EnableMQTT == true && WiFi.status() == WL_CONNECTED) {
    MqttConnectionCheck();
  }

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

  if (abs(VoltageTimer - CurrentTime) > 120000) {
    ReadVoltage();
    VoltageTimer = millis();
  }

  MaxZoneTimeOnCheck();

}


//-----------------------------------------------------------------------------------
//Wifi, AP and BLE
//-----------------------------------------------------------------------------------
void ConnectToDaWEEEEFEEEEEEEE(int Attempts, int Timeout) {
  preferences.begin("credentials", false);
  if (preferences.getString("ssid") != "") {
    if (NumberOfWifiReconntionFailures < Attempts) {
      SerialOutput("Connecting to ", false);
      SerialOutput(preferences.getString("ssid"), false);
      SerialOutput("  Attempt: ", false);
      SerialOutput(String(NumberOfWifiReconntionFailures), true);
      WiFi.begin(preferences.getString("ssid").c_str(), preferences.getString("password").c_str());


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
        WifiReattemptsBeforeAP = 0;
      }
      else {
        NumberOfWifiReconntionFailures += 1;
      }
    }
  }
  else {
    //no ssid turn on AP Mode
    APMode = true;
  }

  preferences.end();
}

void CheckNetworkSettings() {
  //get live network settings
}

void SetupAP() {
  String APName = Name + ID;
  preferences.begin("SystemSettings", true);
  WiFi.softAP(APName.c_str(), preferences.getString("APMode_Password").c_str());
  Serial.print("SSID NAME:");
  Serial.println(APName);
  Serial.print("SSID Password:");
  Serial.println(preferences.getString("APMode_Password"));
  preferences.end();
  IPAddress myIP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(myIP);
}

void DisableAP() {
  WiFi.softAPdisconnect();
}

//-----------------------------------------------------------------------------------
//System Level Functions
//-----------------------------------------------------------------------------------
void RESETEVERYTHING() {
  ClearAllStoredData();
  CheckStoredData();
  ESP.restart();
}

void FlushMemoryCompletely() {
  nvs_flash_erase(); // erase the NVS partition and...
  nvs_flash_init(); // initialize the NVS partition.
}


//-----------------------------------------------------------------------------------
//Storing and Retrieving stuff from memory
//-----------------------------------------------------------------------------------
void PutAPSSIDPassword() {
  //check if password is longer than 7 characters and less than 64
}

void PutWifiSSIDPassword() {
  //check if password is longer than 7 characters and less than 64
}

void PutAPPassword() {
  //check if password is longer than 7 characters and less than 64
}

void PutWifiSSID() {

}

void PutAdminPassword() {

}

void PutMQTTIP() {

}

void PutMQTTPort() {

}

void PutZoneMaxTimeOn(int Zone, float Mins) {
  //check if longer than 7.0 mins and less than 59.99
}

void ClearAllStoredData() {
  preferences.begin("credentials", false);
  preferences.clear();
  preferences.end();
  preferences.begin("SystemSettings", false);
  preferences.clear();
  preferences.end();
}

void CheckStoredData() {
  /*
     Check if the stored data needed to run is there if bot sets it to default.
  */
  preferences.begin("credentials", false);
  if (preferences.isKey("ssid") == false) {
    preferences.putString("ssid", "");
  }

  if (preferences.isKey("password") == false) {
    preferences.putString("password", "");
  }

  if (preferences.isKey("Admin_password") == false) {
    preferences.putString("Admin_password", "SoOriginalThereBoss");
    SerialOutput("No Admin password setting to: " + preferences.getString("Admin_password"), true);
  }

  preferences.end();
  preferences.begin("SystemSettings", false);

  if (preferences.isKey("LocalLockOut") == false) {
    preferences.putBool("LocalLockOut", true);
    SerialOutput("LocalLockOut not defined setting to on", true);
  }

  if (preferences.isKey("Battery") == false) {
    preferences.putBool("Battery", true);
    SerialOutput("Battery Mode not defined setting to On", true);
  }

  if (preferences.isKey("MQTTIP") == false) {
    preferences.putString("MQTTIP", ""); //Tested with IP not hostnames
  }

  if (preferences.isKey("MQTTPORT") == false) {
    preferences.putInt("MQTTPORT", 1883);
  }

  if (preferences.isKey("APMode_Password") == false) {
    preferences.putString("APMode_Password", "MUNAAAYE");
    SerialOutput("No AP Password setting to: " + preferences.getString("APMode_Password"), true);
  }

  if (preferences.isKey("APMode") == false) {
    preferences.putBool("EnableMQTT", false);
    SerialOutput("MQTT not defined setting to off", true);
  }

  if (preferences.isKey("EnableWIFI") == false) {
    preferences.putBool("EnableWIFI", false);
    SerialOutput("Wifi Mode not defined set to off", true);
  }

  if (preferences.isKey("APMode") == false) {
    preferences.putBool("APMode", true);
    SerialOutput("AP mode not defined setting to On", true);
  }

  if (preferences.isKey("Z1_Max") == false) {
    preferences.putFloat("Z1_Max", 7.5);
    SerialOutput("Zone 1 Max On time not defined setting to: " + String(preferences.getFloat("Z1_Max")), true);
  }

  if (preferences.isKey("Z2_Max") == false) {
    preferences.putFloat("Z2_Max", 7.5);
    SerialOutput("Zone 2 Max On time not defined setting to: " + String(preferences.getFloat("Z2_Max")), true);
  }

  if (preferences.isKey("Z3_Max") == false) {
    preferences.putFloat("Z3_Max", 7.5);
    SerialOutput("Zone 3 Max On time not defined setting to: " + String(preferences.getFloat("Z3_Max")), true);
  }

  if (preferences.isKey("Z4_Max") == false) {
    preferences.putFloat("Z4_Max", 7.5);
    SerialOutput("Zone 4 Max On time not defined setting to: " + String(preferences.getFloat("Z4_Max")), true);
  }
  preferences.end();

  preferences.begin("Network_Settings", false);
  if (preferences.isKey("DHCP") == false) {
    preferences.putBool("DHCP", true);
    SerialOutput("", true);
  }
  if (preferences.isKey("GATEWAY") == false) {
    preferences.putString("GATEWAY", "");
  }
  if (preferences.isKey("IP") == false) {
    preferences.putString("IP", "");
  }
  if (preferences.isKey("DNS") == false) {
    preferences.putString("DNS", "");
  }
  if (preferences.isKey("Subnet") == false) {
    preferences.putString("Subnet", "");
  }
  if (preferences.isKey("NTP") == false) {
    preferences.putString("NTP", "");
  }

  preferences.end();

}

//-----------------------------------------------------------------------------------
//Reading states
//-----------------------------------------------------------------------------------
void ReadVoltage() {
  LastVSVoltage = (30.954 / 4095) * analogRead(VSVoltagePin);
  SerialOutput("Voltage:" + String(LastVSVoltage), true);

  if (EnableMQTT == true) {
    String VTopic = "/" + Name + "/" + ID + "/Voltage";
    mqttClient.publish(VTopic.c_str(), String(LastVSVoltage).c_str());
  }
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
    if (EnableMQTT == true) {
      mqttClient.publish(PathName.c_str(), LastZIN1State.c_str());
    }
    SerialOutput("Input:1:" + ReadInput(1), true);

  }

  if (LastZIN2State != ReadInput(2)) {
    PathName = PathName + "2";
    LastZIN2State = String(ReadInput(2));
    if (EnableMQTT == true) {
      mqttClient.publish(PathName.c_str(), LastZIN2State.c_str());
    }
    SerialOutput("Input:2:" + ReadInput(2), true);
  }

  if (LastZIN3State != ReadInput(3)) {
    PathName = PathName + "3";
    LastZIN3State = String(ReadInput(3));
    if (EnableMQTT == true) {
      mqttClient.publish(PathName.c_str(), LastZIN3State.c_str());
    }
    SerialOutput("Input:3:" + ReadInput(3), true);
  }

  if (LastZIN4State != ReadInput(4)) {
    PathName = PathName + "4";
    LastZIN4State = String(ReadInput(4));
    if (EnableMQTT == true) {
      mqttClient.publish(PathName.c_str(), LastZIN4State.c_str());
    }
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
      if (LastMQTTZO1State != ReadOutput(1) && EnableMQTT == true) {
        mqttClient.publish(ZO1Topic.c_str(), String(ReadOutput(1)).c_str());
      }
      break;
    case 2:
      digitalWrite(Zone2Output, State);
      if (State == HIGH) {
        Zone2TurnedOnTime = millis();
      }
      if (LastMQTTZO2State != ReadOutput(2) && EnableMQTT == true) {
        mqttClient.publish(ZO2Topic.c_str(), String(ReadOutput(2)).c_str());
      }
      break;
    case 3:
      digitalWrite(Zone3Output, State);
      if (State == HIGH) {
        Zone3TurnedOnTime = millis();
      }
      if (LastMQTTZO3State != ReadOutput(3) && EnableMQTT == true) {
        mqttClient.publish(ZO3Topic.c_str(), String(ReadOutput(3)).c_str());
      }
      break;
    case 4:
      digitalWrite(Zone4Output, State);
      if (State == HIGH) {
        Zone4TurnedOnTime = millis();
      }

      if (LastMQTTZO4State != ReadOutput(4) && EnableMQTT == true) {
        mqttClient.publish(ZO4Topic.c_str(), String(ReadOutput(4)).c_str());
      }
      break;
  }

  SerialOutput("Zone:" + String(Number) + ":" + ReadOutput(Number), true);
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

String processor(const String& var) {
  // Place holder
  Serial.println(var);
  return String();
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
      }
      if (message == "off") {
        SetOutput(1, false);
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
      }
      if (message == "off") {
        SetOutput(2, false);
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
      }
      if (message == "off") {
        SetOutput(3, false);
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
      }
      if (message == "off") {
        SetOutput(4, false);
      }
    }
  }

}

void webserverAPI() {

  // Route for root / web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(SPIFFS, "/index.html", String(), false, processor);
  });

  server.on("/styles.css", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(SPIFFS, "/styles.css", "text/css");
  });

  server.on("/main.js", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(SPIFFS, "/main.js", "text/javascript");
  });

  server.on("/polyfills.js", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(SPIFFS, "/polyfills.js", "text/javascript");
  });

  server.on("/runtime.js", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(SPIFFS, "/runtime.js", "text/javascript");
  });

  server.on("/favicon.ico", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(SPIFFS, "/favicon.ico", "image/x-icon");
  });

}
