#include <Adafruit_MCP23X08.h>

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
String Version = "0.0.5";
bool EnableMQTT, APMode, EnableWifi, Battery, LocalControlLockOut, APEnabled, LastLocalControlLockOut, ZoneExpansionDaughterboard = false;
String Name = "MakeItRain";
String ID;
int NumberOfWifiReconntionFailures = 0;
int MaxAttempts = 4;
int WifiReattemptsBeforeAP = 0;
Preferences preferences;
long VoltageTimer, WifiTryAgainTimer, MinTimeOnTimer, BetweenWifiAttempts, DebugTimer;

#define VSVoltagePin 33
#define AUXVoltagePin 32
#define ResetButton 25
#define LEDOut 27
float LastVSVoltage;

//Zone definitions
Adafruit_MCP23X08 GPIOCHIPITYCHIPCHIP;
bool GrootToGo = true;

#define Zone1Input 34
String ZO1Topic = "";
String LastMQTTZO1State = "off";
String LastZIN1State;
float ZO1MaxOn;
long Zone1TurnedOnTime;

#define Zone2Input 35
String ZO2Topic = "";
String LastMQTTZO2State = "off";
String LastZIN2State;
float ZO2MaxOn;
long Zone2TurnedOnTime;

#define Zone3Input 36
String ZO3Topic = "";
String LastMQTTZO3State = "off";
String LastZIN3State;
float ZO3MaxOn;
long Zone3TurnedOnTime;

#define Zone4Input 39
String ZO4Topic = "";
String LastMQTTZO4State = "off";
String LastZIN4State;
float ZO4MaxOn;
long Zone4TurnedOnTime;

void setup() {
  Serial.begin(115200);
  SerialOutput("Starting to... MAKEITRAIN  Version: " + Version, true);
  //WriteSomeDataForMeUntilIGetWEbWorking();
  CheckStoredData();

  //Zones
  pinMode(Zone1Input, INPUT);
  pinMode(Zone2Input, INPUT);
  pinMode(Zone3Input, INPUT);
  pinMode(Zone4Input, INPUT);

  if (!GPIOCHIPITYCHIPCHIP.begin_I2C()) {
    GrootToGo = false;
  }
  else {
    GPIOCHIPITYCHIPCHIP.pinMode(0, OUTPUT); //Zone 1
    GPIOCHIPITYCHIPCHIP.pinMode(1, OUTPUT); //Zone 2
    GPIOCHIPITYCHIPCHIP.pinMode(2, OUTPUT); //Zone 3
    GPIOCHIPITYCHIPCHIP.pinMode(3, OUTPUT); //Zone 4

    GPIOCHIPITYCHIPCHIP.digitalWrite(0, 0); //Zone 1 Turnning off
    GPIOCHIPITYCHIPCHIP.digitalWrite(1, 0); //Zone 2 Turnning off
    GPIOCHIPITYCHIPCHIP.digitalWrite(2, 0); //Zone 3 Turnning off
    GPIOCHIPITYCHIPCHIP.digitalWrite(3, 0); //Zone 4 Turnning off

    //turning off the remaining IO on the MCP chip after setting them to output mode
    GPIOCHIPITYCHIPCHIP.pinMode(4, OUTPUT); //Z5
    GPIOCHIPITYCHIPCHIP.digitalWrite(4, 0); //Z5 Turnning off
    GPIOCHIPITYCHIPCHIP.pinMode(5, OUTPUT); //Z6
    GPIOCHIPITYCHIPCHIP.digitalWrite(5, 0); //Z6 Turnning off
    GPIOCHIPITYCHIPCHIP.pinMode(6, OUTPUT); //Z7
    GPIOCHIPITYCHIPCHIP.digitalWrite(6, 0); //Z7 Turnning off
    GPIOCHIPITYCHIPCHIP.pinMode(7, OUTPUT); //Z8
    GPIOCHIPITYCHIPCHIP.digitalWrite(7, 0); //Z8 Turnning off
  }

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

  if (preferences.getBool("IDOverride") == false) {
    String MAC = WiFi.macAddress();
    for (int x = 9; x < 17; x++) {
      if (MAC.charAt(x) != ':') {
        ID.concat(MAC.charAt(x));
      }
    }
  }
  else {
    ID = preferences.getString("ID");
  }
  preferences.end();

  if (EnableWifi == true) {
    ConnectToDaWEEEEFEEEEEEEE(1, 60000);
  }

  if (EnableMQTT == true) {
    SetupMQTT();
  }

  if (APMode == true || EnableWifi == false) {
    //Turn on Bluetooth and put Wifi into AP mode
    SetupAP();
  }

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
    /*
       basic workflow for wifi connection management
       if disconnected from wifi it will attempt to connect with 2 min inbetween
       after MaxAttempts is exhausted it will space trying to connect to 15 min
       intervals. If APmode is off it will turn on AP mode to allow for debugging
       and control. If APmode is disabled when the wifi reconnects that will be
       turned back off.
    */
    if (WifiReattemptsBeforeAP < MaxAttempts && abs(BetweenWifiAttempts - CurrentTime) > 1200000) {
      ConnectToDaWEEEEFEEEEEEEE(MaxAttempts, 60000);
      BetweenWifiAttempts = millis();
    }

    if (WiFi.status() != WL_CONNECTED) {
      WifiReattemptsBeforeAP += 1;
    }

    if (WifiReattemptsBeforeAP >= MaxAttempts) {
      APMode = true;
      if (abs(WifiTryAgainTimer - CurrentTime) > 900000) {
        NumberOfWifiReconntionFailures = 0;
        WifiTryAgainTimer = millis();
      }
    }
  }

  if (EnableMQTT == true && WiFi.status() == WL_CONNECTED) {
    MqttConnectionCheck();
  }

  if (EnableWifi == false && APMode == false) {
    APMode = true;
  }

  if (APMode == true && APEnabled == false) {
    //Turn on AP mode
    SetupAP();
  }

  if (APMode == false && APEnabled == true) {
    // if AP is enabled and it's suppose to be off turn it off
    DisableAP();
  }

  /* Enable/Disable the local input Interrupts
      If they are disabled they will be polled and pushed to MQTT for state
  */
  if (LocalControlLockOut != LastLocalControlLockOut) {
    LocalInputs(LocalControlLockOut);
    LastLocalControlLockOut = LocalControlLockOut;
  }

  if (abs(VoltageTimer - CurrentTime) > 120000) {
    ReadVoltage();
    VoltageTimer = millis();
  }

  MaxZoneTimeOnCheck();

  if (Battery == true) {
    // Check if any of the zones are on. if any are on do not go to sleep
    // Check if the Min Time On time has been exhausted true = go to sleep
    // Do a last min read of Voltage and any other status that needs to get posted
    // Set RTC to Sleep for 30 Mins
  }

  if (abs(DebugTimer - CurrentTime) > 5000) {
    Serial.println("Mic Check");
    DebugTimer = millis();
  }

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
      WiFi.begin(preferences.getString("ssid").c_str(), preferences.getString("ssid_password").c_str());


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
        Serial.print("WIFI IP: ");
        Serial.println(WiFi.localIP());
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
  APEnabled = true;
}

void DisableAP() {
  APEnabled = false;
  WiFi.softAPdisconnect();
}

void SetupBT() {

}

void DisableBT() {

}

//-----------------------------------------------------------------------------------
//System Level Functions
//-----------------------------------------------------------------------------------
void RESETEVERYTHING() {
  ClearAllStoredData();
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

void WriteSomeDataForMeUntilIGetWEbWorking() {
  preferences.begin("credentials", false);
  //  preferences.putString("ssid", "");
  //  preferences.putString("password", "");
  //  preferences.putString("Admin_password", "SoOriginalThereBoss");
  preferences.end();


  preferences.begin("SystemSettings", false);
  //preferences.putBool("LocalLockOut", true);
  //preferences.putBool("Battery", true);
  //preferences.putString("MQTTIP", ""); //Tested with IP not hostnames
  //preferences.putInt("MQTTPORT", 1883);
  //preferences.putString("APMode_Password", "MUNAAAYE");
  //preferences.putBool("EnableMQTT", true);
  //  preferences.putBool("EnableWIFI", true);
  //  preferences.putBool("APMode", true);
  //  preferences.putFloat("Z1_Max", 7.5);
  //  preferences.putFloat("Z2_Max", 7.5);
  //  preferences.putFloat("Z3_Max", 7.5);
  //  preferences.putFloat("Z4_Max", 7.5);
  //preferences.putString("ID", "");
  //preferences.putBool("IDOverride", true);
  preferences.end();

  preferences.begin("Network_Settings", false);
  //  preferences.putBool("DHCP", true);
  //  preferences.putString("GATEWAY", "");
  //  preferences.putString("IP", "");
  //  preferences.putString("DNS", "");
  //  preferences.putString("Subnet", "");
  //  preferences.putString("NTP", "");
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

  if (preferences.isKey("ssid_password") == false) {
    preferences.putString("ssid_password", "");
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

  if (preferences.isKey("EnableMQTT") == false) {
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

  if (preferences.isKey("IDOverride") == false) {
    preferences.putBool("IDOverride", false);
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

  String VTopic = "/" + Name + "/" + ID + "/Voltage";
  MQTTSend(VTopic, String(LastVSVoltage));
}

String ReadOutput(int Number) {
  /*

  */
  String ValueToReturn = "off";
  switch (Number) {
    case 1:
      if (GPIOCHIPITYCHIPCHIP.digitalRead(0) == 1) {
        ValueToReturn = "on";
      }
      break;
    case 2:
      if (GPIOCHIPITYCHIPCHIP.digitalRead(1) == 1) {
        ValueToReturn = "on";
      }
      break;
    case 3:
      if (GPIOCHIPITYCHIPCHIP.digitalRead(2) == 1) {
        ValueToReturn = "on";
      }
      break;
    case 4:
      if (GPIOCHIPITYCHIPCHIP.digitalRead(3) == 1) {
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
    MQTTSend(PathName, LastZIN1State);
    SerialOutput("Input:1:" + ReadInput(1), true);

  }

  if (LastZIN2State != ReadInput(2)) {
    PathName = PathName + "2";
    LastZIN2State = String(ReadInput(2));
    MQTTSend(PathName, LastZIN2State);
    SerialOutput("Input:2:" + ReadInput(2), true);
  }

  if (LastZIN3State != ReadInput(3)) {
    PathName = PathName + "3";
    LastZIN3State = String(ReadInput(3));
    MQTTSend(PathName, LastZIN3State);
    SerialOutput("Input:3:" + ReadInput(3), true);
  }

  if (LastZIN4State != ReadInput(4)) {
    PathName = PathName + "4";
    LastZIN4State = String(ReadInput(4));
    MQTTSend(PathName, LastZIN4State);
    SerialOutput("Input:4:" + ReadInput(4), true);
  }


}
//-----------------------------------------------------------------------------------
//Local control interrupts
//-----------------------------------------------------------------------------------
void LocalInputs(bool State) {
  if (State == true) {
    attachInterrupt(digitalPinToInterrupt(Zone1Input), LocalInput1, FALLING);
    attachInterrupt(digitalPinToInterrupt(Zone2Input), LocalInput2, FALLING);
    attachInterrupt(digitalPinToInterrupt(Zone3Input), LocalInput3, FALLING);
    attachInterrupt(digitalPinToInterrupt(Zone4Input), LocalInput4, FALLING);
    interrupts();
  }
  else {
    detachInterrupt(digitalPinToInterrupt(Zone1Input));
    detachInterrupt(digitalPinToInterrupt(Zone2Input));
    detachInterrupt(digitalPinToInterrupt(Zone3Input));
    detachInterrupt(digitalPinToInterrupt(Zone4Input));
    noInterrupts();
  }
}

bool ReadMCP23008(int PortNumber) {
  bool PortState = GPIOCHIPITYCHIPCHIP.digitalRead(PortNumber);
  return PortState;
}

void TogglePort(int PortNumber) {
  if (GrootToGo == true) {
    if (GPIOCHIPITYCHIPCHIP.digitalRead(PortNumber) == LOW) {
      SetOutput(PortNumber, HIGH);
    }
    else {
      SetOutput(PortNumber, LOW);
    }
  }
  else {
    //put message for can't talk to mcp23008
  }
}

void LocalInput1() {
  TogglePort(1);
}

void LocalInput2() {
  TogglePort(2);
}

void LocalInput3() {
  TogglePort(3);
}

void LocalInput4() {
  TogglePort(4);
}

//-----------------------------------------------------------------------------------
//Set Functions
//-----------------------------------------------------------------------------------
void SetOutput(int Number, bool State) {
  /*

  */
  if (GrootToGo == true) {
    switch (Number) {
      case 1:
        GPIOCHIPITYCHIPCHIP.digitalWrite(0, State);
        if (State == HIGH) {
          Zone1TurnedOnTime = millis();
        }
        if (LastMQTTZO1State != ReadOutput(1)) {
          MQTTSend(ZO1Topic, String(ReadOutput(1)));
        }
        break;
      case 2:
        GPIOCHIPITYCHIPCHIP.digitalWrite(1, State);
        if (State == HIGH) {
          Zone2TurnedOnTime = millis();
        }
        if (LastMQTTZO2State != ReadOutput(2)) {
          MQTTSend(ZO2Topic, String(ReadOutput(2)));
        }
        break;
      case 3:
        GPIOCHIPITYCHIPCHIP.digitalWrite(2, State);
        if (State == HIGH) {
          Zone3TurnedOnTime = millis();
        }
        if (LastMQTTZO3State != ReadOutput(3)) {
          MQTTSend(ZO3Topic, String(ReadOutput(3)));
        }
        break;
      case 4:
        GPIOCHIPITYCHIPCHIP.digitalWrite(3, State);
        if (State == HIGH) {
          Zone4TurnedOnTime = millis();
        }
        if (LastMQTTZO4State != ReadOutput(4)) {
          MQTTSend(ZO4Topic, String(ReadOutput(4)));
        }
        break;
    }

    SerialOutput("Zone:" + String(Number) + ":" + ReadOutput(Number), true);
  }
  else {
    SerialOutput("Can Not Communicate with MCP23008", true);
  }
}

//-----------------------------------------------------------------------------------
//Timer Related
//-----------------------------------------------------------------------------------

void MaxZoneTimeOnCheck() {
  long CurrentTime = millis();
  if (GPIOCHIPITYCHIPCHIP.digitalRead(0) == HIGH && abs(CurrentTime - Zone1TurnedOnTime) >= ZO1MaxOn * 60000) {
    SetOutput(1, LOW);
  }

  if (GPIOCHIPITYCHIPCHIP.digitalRead(1) == HIGH && abs(CurrentTime - Zone2TurnedOnTime) >= ZO2MaxOn * 60000) {
    SetOutput(2, LOW);
  }

  if (GPIOCHIPITYCHIPCHIP.digitalRead(2) == HIGH && abs(CurrentTime - Zone3TurnedOnTime) >= ZO3MaxOn * 60000) {
    SetOutput(3, LOW);
  }

  if (GPIOCHIPITYCHIPCHIP.digitalRead(3) == HIGH && abs(CurrentTime - Zone4TurnedOnTime) >= ZO4MaxOn * 60000) {
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
  if (!mqttClient.connected()) {
    reconnect();
  }
  mqttClient.loop();
}

void MQTTSend(String Topic, String Payload) {
  if (EnableMQTT == true && mqttClient.connected()) {
    mqttClient.publish(Topic.c_str(), Payload.c_str());
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
      //mqttClient.publish(ZO1Topic.c_str(), String(ReadOutput(1)).c_str());
      ZO2Topic =  "/" + Name + "/" + ID + "/ZoneOutput/2";
      mqttClient.subscribe(ZO2Topic.c_str());
      //mqttClient.publish(ZO2Topic.c_str(), String(ReadOutput(2)).c_str());
      ZO3Topic =  "/" + Name + "/" + ID + "/ZoneOutput/3";
      mqttClient.subscribe(ZO3Topic.c_str());
      //mqttClient.publish(ZO3Topic.c_str(), String(ReadOutput(3)).c_str());
      ZO4Topic =  "/" + Name + "/" + ID + "/ZoneOutput/4";
      mqttClient.subscribe(ZO4Topic.c_str());
      //mqttClient.publish(ZO4Topic.c_str(), String(ReadOutput(4)).c_str());
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
