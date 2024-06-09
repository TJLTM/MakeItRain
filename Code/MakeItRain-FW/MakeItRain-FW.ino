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
//External RTC
#include "RTClib.h"
PROGMEM const char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};
RTC_DS3231 rtc;
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
String Version PROGMEM = "0.1.2";
//Target HW Version for this code is v1.4.0 and greater
bool EnableMQTT, RTCSense, APMode, EnableWifi, Battery, LocalControlToggle, APEnabled, LastLocalControlToggle, ZoneExpansionDaughterboard, firstRun, IsMQTTSetup, TurnOffAPModeWhenWifiIsBack = false;
String Name PROGMEM = "MakeItRain";
String ID;
String BaseMQTTTopicString = "";
int NumberOfWifiReconntionFailures = 0;
#define MaxAttempts 4
int WifiReattemptsBeforeAP = 0;
Preferences preferences;
long VoltageTimer, WifiTryAgainTimer, BetweenWifiAttempts, FiveSecondTimer;

#define VSVoltagePin 33
#define AUXVoltagePin 32
#define ResetButton 25
#define LEDOut 27

#define Input1 34
#define Input2 35
#define Input3 36
#define Input4 39

#define MAXOutputZones 4
long LastZoneStartTime[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };
float MaxZoneOnTime[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };
bool LastInputStates[4] = { 0, 0, 0, 0 };
bool LastMQTTState[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };

//Zone definitions
Adafruit_MCP23X08 GPIOCHIPITYCHIPCHIP;
bool GrootToGo = true;

void setup() {
  Serial.begin(115200);
  delay(250);
  Serial.println("Starting to... MAKEITRAIN  Version: " + Version);
  CheckStoredData();

  //Inputs
  pinMode(Input1, INPUT);
  pinMode(Input2, INPUT);
  pinMode(Input3, INPUT);
  pinMode(Input4, INPUT);

  //Outputs
  if (!GPIOCHIPITYCHIPCHIP.begin_I2C()) {
    GrootToGo = false;
    Serial.println("Can Not Communicate with MCP23008");
  } else {
    GPIOCHIPITYCHIPCHIP.pinMode(0, OUTPUT);  //Zone 1
    GPIOCHIPITYCHIPCHIP.pinMode(1, OUTPUT);  //Zone 2
    GPIOCHIPITYCHIPCHIP.pinMode(2, OUTPUT);  //Zone 3
    GPIOCHIPITYCHIPCHIP.pinMode(3, OUTPUT);  //Zone 4

    GPIOCHIPITYCHIPCHIP.digitalWrite(0, 0);  //Zone 1 Turnning off
    GPIOCHIPITYCHIPCHIP.digitalWrite(1, 0);  //Zone 2 Turnning off
    GPIOCHIPITYCHIPCHIP.digitalWrite(2, 0);  //Zone 3 Turnning off
    GPIOCHIPITYCHIPCHIP.digitalWrite(3, 0);  //Zone 4 Turnning off

    //turning off the remaining IO on the MCP chip after setting them to output mode
    GPIOCHIPITYCHIPCHIP.pinMode(4, OUTPUT);  //Z5
    GPIOCHIPITYCHIPCHIP.digitalWrite(4, 0);  //Z5 Turnning off
    GPIOCHIPITYCHIPCHIP.pinMode(5, OUTPUT);  //Z6
    GPIOCHIPITYCHIPCHIP.digitalWrite(5, 0);  //Z6 Turnning off
    GPIOCHIPITYCHIPCHIP.pinMode(6, OUTPUT);  //Z7
    GPIOCHIPITYCHIPCHIP.digitalWrite(6, 0);  //Z7 Turnning off
    GPIOCHIPITYCHIPCHIP.pinMode(7, OUTPUT);  //Z8
    GPIOCHIPITYCHIPCHIP.digitalWrite(7, 0);  //Z8 Turnning off
    Serial.println("Finished setting up GPIO");
  }

  if (rtc.begin()) {
    RTCSense = true;
  }


  Serial.println("Loading System Settings");
  preferences.begin("SystemSettings", true);
  //setup other System Level settings
  LocalControlToggle = preferences.getBool("LocalControlToggle");

  MaxZoneOnTime[0] = preferences.getFloat("Z1_Max");
  MaxZoneOnTime[1] = preferences.getFloat("Z2_Max");
  MaxZoneOnTime[2] = preferences.getFloat("Z3_Max");
  MaxZoneOnTime[3] = preferences.getFloat("Z4_Max");
  MaxZoneOnTime[4] = preferences.getFloat("Z5_Max");
  MaxZoneOnTime[5] = preferences.getFloat("Z6_Max");
  MaxZoneOnTime[6] = preferences.getFloat("Z7_Max");
  MaxZoneOnTime[7] = preferences.getFloat("Z8_Max");

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
  } else {
    ID = preferences.getString("ID");
  }
  preferences.end();

  Serial.println("Finished Loading system settings");

  BaseMQTTTopicString = "/" + Name + "/" + ID + "/";
  Serial.print("Base MQTT Topic:");
  Serial.println(BaseMQTTTopicString);

  if (EnableWifi == true) {
    ConnectToDaWEEEEFEEEEEEEE(10000);
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
  } else {
    webserverAPI();
    server.begin();
  }
}

void loop() {
  long CurrentTime = millis();

  if (abs(FiveSecondTimer - CurrentTime) > 5000) {
    Serial.print("EnableWifi:");
    Serial.println(EnableWifi);
    FiveSecondTimer = CurrentTime;
  }

  if (EnableWifi == true) {
    /*
       basic workflow for wifi connection management
       if disconnected from wifi it will attempt to connect with 30 seconds inbetween
       after MaxAttempts is exhausted it will space trying to connect to 7.5 min
       intervals. If APmode is off it will turn on AP mode to allow for debugging
       and control. If APmode is disabled when the wifi reconnects that will be
       turned back off.
    */
    if (WiFi.status() != WL_CONNECTED) {
      if (firstRun == false) {
        if (EnableMQTT == true) {
          mqttClient.disconnect();
          IsMQTTSetup = false;
        }
      }

      if (WifiReattemptsBeforeAP <= MaxAttempts) {
        if (abs(BetweenWifiAttempts - CurrentTime) > 30000 || firstRun == false) {
          ConnectToDaWEEEEFEEEEEEEE(5000);
          BetweenWifiAttempts = millis();
          WifiReattemptsBeforeAP += 1;
          firstRun = true;
          WifiTryAgainTimer = millis();
        }
      } else {
        firstRun = false;
      }

      if (WifiReattemptsBeforeAP >= MaxAttempts) {
        if (APMode != true) {
          TurnOffAPModeWhenWifiIsBack = true;
          APMode = true;
        }
        if (abs(WifiTryAgainTimer - CurrentTime) > 450000) {
          NumberOfWifiReconntionFailures = 0;
        }
      }
    } else {
      NumberOfWifiReconntionFailures = 0;
      WifiReattemptsBeforeAP = 0;
      if (TurnOffAPModeWhenWifiIsBack == true && APMode == true) {
        APMode = false;
      }
    }
  }

  /*
     Handle MQTT Connection
  */

  if (EnableMQTT == true && WiFi.status() == WL_CONNECTED) {
    MqttConnectionCheck();
  }


  /*
     Handle AP mode
     handles the cases of turning it on and off
     and turning it on if Wifi is turned off and APmode is also off
  */
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
  //  if (LocalControlToggle != LastLocalControlToggle) {
  //    LocalInputs(LocalControlToggle);
  //    LastLocalControlToggle = LocalControlToggle;
  //  }
  //
  //  if (LocalControlToggle == false) {
  //    CheckIfInputsHaveChanged();
  //  }

  if (abs(VoltageTimer - CurrentTime) > 60000) {
    ReadVoltage();
    ReadAUXVoltage();
    VoltageTimer = millis();
  }

  MaxZoneTimeOnCheck();

  if (Battery == true) {
    // Check if any of the zones are on. if any are on do not go to sleep
    // Check if the Min Time On time has been exhausted true = go to sleep
    // Do a last min read of Voltage and any other status that needs to get posted
    // Set RTC to Sleep for 30 Mins
  }
}


//-----------------------------------------------------------------------------------
//Wifi, AP and BLE
//-----------------------------------------------------------------------------------
void ConnectToDaWEEEEFEEEEEEEE(int Timeout) {
  preferences.begin("credentials", false);
  if (preferences.getString("ssid") != "") {
    if (preferences.getString("ssid_password") != "") {
      WiFi.begin(preferences.getString("ssid").c_str(), preferences.getString("ssid_password"));
      String hostname = "MIR" + ID;
      WiFi.setHostname(hostname.c_str());
      Serial.print("Connecting to WIFI ssid:");
      Serial.println(preferences.getString("ssid"));
      int StartTime = millis();
      int CurrentTime = millis();
      while (WiFi.status() != WL_CONNECTED && abs(StartTime - CurrentTime) < Timeout) {
        delay(500);
        CurrentTime = millis();
        Serial.print(".");
      }
      Serial.println("");
      if (WiFi.status() == WL_CONNECTED) {
        Serial.println("WIFI IS CONNECTED!");
        Serial.print("IP Assigned: ");
        Serial.println(WiFi.localIP());
        Serial.print("Hostname: ");
        Serial.println(WiFi.getHostname());
      } else {
        Serial.println("WIFI CONNECTION FAILED!");
      }
    } else {
      //no ssid turn on AP Mode
      Serial.println("No SSID Password stored");
      APMode = true;
    }
  } else {
    //no ssid turn on AP Mode
    Serial.println("No SSID stored");
    APMode = true;
  }

  preferences.end();
}

void SetupAP() {
  String APName = Name + ID;
  preferences.begin("SystemSettings", true);
  Serial.print("Starting AP:");
  Serial.println(APName);
  WiFi.softAP(APName.c_str(), preferences.getString("APMode_Password").c_str());
  preferences.end();
  IPAddress myIP = WiFi.softAPIP();
  APEnabled = true;
}

void DisableAP() {
  APEnabled = false;
  Serial.print("Stopping AP");
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
  nvs_flash_erase();  // erase the NVS partition and...
  nvs_flash_init();   // initialize the NVS partition.
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

  if (preferences.isKey("ssid_password") == false) {
    preferences.putString("ssid_password", "");
  }

  if (preferences.isKey("Admin_password") == false) {
    preferences.putString("Admin_password", "SoOriginalThereBoss");
    Serial.print("No Admin Password found setting default");
  }

  preferences.end();
  preferences.begin("SystemSettings", false);

  if (preferences.isKey("LocalControlToggle") == false) {
    preferences.putBool("LocalControlToggle", true);
    Serial.println("LocalControlToggle setting not found setting default");
  }

  if (preferences.isKey("Battery") == false) {
    preferences.putBool("Battery", false);
    Serial.println("Battery setting not found setting default");
  }

  if (preferences.isKey("MQTTIP") == false) {
    preferences.putString("MQTTIP", "");  //Tested with IP not hostnames
    Serial.println("MQTTIP setting not found setting default");
  }

  if (preferences.isKey("MQTTPORT") == false) {
    preferences.putInt("MQTTPORT", 1883);
    Serial.println("MQTTPORT setting not found setting default");
  }

  if (preferences.isKey("APMode_Password") == false) {
    preferences.putString("APMode_Password", "MUNAAAYE");
    Serial.println("APMode_Password not found setting default");
  }

  if (preferences.isKey("EnableMQTT") == false) {
    preferences.putBool("EnableMQTT", false);
    Serial.println("EnableMQTT not found setting default");
  }

  if (preferences.isKey("EnableWIFI") == false) {
    preferences.putBool("EnableWIFI", false);
    Serial.println("EnableWifi not found setting default");
  }

  if (preferences.isKey("APMode") == false) {
    preferences.putBool("APMode", true);
    Serial.println("APMode not found setting default");
  }

  if (preferences.isKey("Z1_Max") == false) {
    preferences.putFloat("Z1_Max", 7.5);
  }

  if (preferences.isKey("Z2_Max") == false) {
    preferences.putFloat("Z2_Max", 7.5);
  }

  if (preferences.isKey("Z3_Max") == false) {
    preferences.putFloat("Z3_Max", 7.5);
  }

  if (preferences.isKey("Z4_Max") == false) {
    preferences.putFloat("Z4_Max", 7.5);
  }

  if (preferences.isKey("Z5_Max") == false) {
    preferences.putFloat("Z5_Max", 7.5);
  }

  if (preferences.isKey("Z6_Max") == false) {
    preferences.putFloat("Z6_Max", 7.5);
  }

  if (preferences.isKey("Z7_Max") == false) {
    preferences.putFloat("Z7_Max", 7.5);
  }

  if (preferences.isKey("Z8_Max") == false) {
    preferences.putFloat("Z8_Max", 7.5);
  }

  if (preferences.isKey("IDOverride") == false) {
    preferences.putBool("IDOverride", false);
  }

  preferences.end();

  preferences.begin("Network_Settings", false);
  if (preferences.isKey("DHCP") == false) {
    preferences.putBool("DHCP", true);
    Serial.println("");
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
  float LastVSVoltage = (32.5 / 4095) * analogRead(VSVoltagePin);
  Serial.print("Voltage: ");
  Serial.println(LastVSVoltage);

  String VTopic = BaseMQTTTopicString + "Voltage";
  MQTTSend(VTopic, String(LastVSVoltage));
}

void ReadAUXVoltage() {
  /*
     AUX Voltage is limited to a range of 0-3.3v. If you need to scale that to other
     Volatage ranges then use onboard pot RV1 to scale it apporpreatly. RV1 = 500k
     Multiturn linear pot
  */
  float LastAuxVoltage = (3.3 / 4095) * analogRead(AUXVoltagePin);
  Serial.print("AUX Voltage: ");
  Serial.println(LastAuxVoltage);

  String VTopic = BaseMQTTTopicString + "AUXVoltage";
  MQTTSend(VTopic, String(LastAuxVoltage));
}

bool ReadOutput(int Number) {
  /*

  */
  bool ValueToReturn = false;
  if (GPIOCHIPITYCHIPCHIP.digitalRead(Number) == 1) {
    ValueToReturn = true;
  }
  return ValueToReturn;
}

bool ReadInput(int Number) {
  /*

  */
  bool ValueToReturn = false;
  switch (Number) {
    case 1:
      ValueToReturn = digitalRead(Input1);
      break;
    case 2:
      ValueToReturn = digitalRead(Input2);
      break;
    case 3:
      ValueToReturn = digitalRead(Input3);
      break;
    case 4:
      ValueToReturn = digitalRead(Input4);
      break;
  }

  return ValueToReturn;
}

void CheckIfInputsHaveChanged() {
  String PathName = BaseMQTTTopicString + "ZoneInput/";
  for (int x = 0; x <= 4; x++) {
    if (LastInputStates[x] != ReadInput(x + 1)) {
      String CurrentInput = PathName + String(x + 1);
      LastInputStates[x] = ReadInput(x + 1);
      MQTTSend(CurrentInput, BoolToString(LastInputStates[x]));
    }
  }
}
//-----------------------------------------------------------------------------------
//Local control interrupts
//-----------------------------------------------------------------------------------
void LocalInputs(bool State) {
  if (State == true) {
    attachInterrupt(digitalPinToInterrupt(Input1), LocalInput1, RISING);
    attachInterrupt(digitalPinToInterrupt(Input2), LocalInput2, RISING);
    attachInterrupt(digitalPinToInterrupt(Input3), LocalInput3, RISING);
    attachInterrupt(digitalPinToInterrupt(Input4), LocalInput4, RISING);
    interrupts();
  } else {
    detachInterrupt(digitalPinToInterrupt(Input1));
    detachInterrupt(digitalPinToInterrupt(Input2));
    detachInterrupt(digitalPinToInterrupt(Input3));
    detachInterrupt(digitalPinToInterrupt(Input4));
    noInterrupts();
  }
}

void TogglePort(int PortNumber) {
  if (GrootToGo == true) {
    if (GPIOCHIPITYCHIPCHIP.digitalRead(PortNumber) == LOW) {
      SetOutput(PortNumber, HIGH);
    } else {
      SetOutput(PortNumber, LOW);
    }
  }
}

void LocalInput1() {
  TogglePort(0);
}

void LocalInput2() {
  TogglePort(1);
}

void LocalInput3() {
  TogglePort(2);
}

void LocalInput4() {
  TogglePort(3);
}

//-----------------------------------------------------------------------------------
//Set Functions
//-----------------------------------------------------------------------------------
void SetOutput(int Number, bool State) {
  /*

  */
  if (GrootToGo == true) {
    String OutputTopic = BaseMQTTTopicString + "ZoneOutput/" + String(Number + 1);
    bool CurrentPortState = ReadOutput(Number);
    //Serial.print("SetOutput ");
    //Serial.print(CurrentPortState);
    //Serial.print("  ");
    //Serial.print(State);
    //Serial.print("   ");
    if (CurrentPortState != State) {
      if (State == HIGH) {
        //Serial.print("Starting Zone Timer");
        LastZoneStartTime[Number] = millis();
      }
      GPIOCHIPITYCHIPCHIP.digitalWrite(Number, State);
      CurrentPortState = ReadOutput(Number);
      //Serial.print(CurrentPortState);
      //Serial.print("  ");
      //Serial.println(LastMQTTState[Number]);
      if (LastMQTTState[Number] != CurrentPortState) {
        MQTTSend(OutputTopic, BoolToString(ReadOutput(Number)));
      }
    }
  }
}

//-----------------------------------------------------------------------------------
//Timer Related
//-----------------------------------------------------------------------------------

void MaxZoneTimeOnCheck() {
  if (GrootToGo == true) {
    long CurrentTime = millis();
    for (int x = 0; x < MAXOutputZones; x++) {
      if (ReadOutput(x) == HIGH && abs(CurrentTime - LastZoneStartTime[x]) >= MaxZoneOnTime[x] * 60000) {
        Serial.print("Zone Max time reached,");
        Serial.print(MaxZoneOnTime[x] * 60000);
        Serial.print(" Turning Off: ");
        Serial.println(x + 1);
        SetOutput(x, LOW);
      }
    }
  }
}

//-----------------------------------------------------------------------------------
//Communication (MQTT/Serial/WEB)
//-----------------------------------------------------------------------------------
String BoolToString(bool State) {
  String StringState = "off";
  if (State == true) {
    StringState = "on";
  }
  return StringState;
}

bool MQTTtoBool(String State) {
  bool BoolState = false;
  if (State == "on") {
    BoolState = true;
  }
  return BoolState;
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
  char* mqttServer;
  mqttServer = &Target[0];
  mqttClient.setServer(mqttServer, preferences.getInt("MQTTPORT"));
  mqttClient.setCallback(callback);
  IsMQTTSetup = true;
  preferences.end();
}

void MqttConnectionCheck() {
  if (IsMQTTSetup == false) {
    SetupMQTT();
  }
  if (!mqttClient.connected()) {
    reconnect();
  }
  mqttClient.loop();
}

void MQTTSend(String Topic, String Payload) {
  if (EnableMQTT == true && mqttClient.connected()) {
    mqttClient.publish(Topic.c_str(), Payload.c_str());
    Serial.print("Sending: ");
    Serial.print(Payload);
    Serial.print("  To Topic: ");
    Serial.println(Topic);
  }
}

void reconnect() {
  // Loop until we're reconnected
  int MQttReconnect = 0;
  while (!mqttClient.connected() && MQttReconnect < 5) {
    // Attempt to connect
    mqttClient.disconnect();
    SetupMQTT();

    if (mqttClient.connect(ID.c_str())) {
      for (int x = 0; x <= MAXOutputZones; x++) {
        String CurrentTopic = BaseMQTTTopicString + "ZoneOutput/" + String(x + 1);
        mqttClient.subscribe(CurrentTopic.c_str());
      }
    } else {
      // Wait 5 seconds before retrying
      delay(5000);
      //This may have to be changed to be non-blocking
    }
    MQttReconnect += 1;
  }
}

void callback(char* topic, byte* payload, unsigned int length) {
  String message;
  for (int i = 0; i < length; i++) {
    message += (char)payload[i];
  }
  Serial.print("MQTT Message:");
  Serial.print(topic);
  Serial.print("   :    ");
  Serial.println(message);

  for (int x = 0; x <= MAXOutputZones; x++) {
    String CurrentTopic = BaseMQTTTopicString + "ZoneOutput/" + String(x + 1);
    if (String((char*)topic) == CurrentTopic) {
      LastMQTTState[x] = MQTTtoBool(message);
      SetOutput(x, MQTTtoBool(message));
      break;
    }
  }
}

void webserverAPI() {
}
