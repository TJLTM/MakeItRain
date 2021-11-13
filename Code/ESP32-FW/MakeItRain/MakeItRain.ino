#include <PubSubClient.h>
#include <Wire.h>
#include <WiFi.h>
#include <HTTP_Method.h>
#include <Uri.h>
#include <WebServer.h>
#include <Preferences.h>
#include <Math.h>


// MQTT client
WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient);
char *mqttServer = "10.10.0.2";
int mqttPort = 1883;

//System Level
String Name = "MakeItRain";
Preferences preferences;
long ThirtyMinTimer;
bool LocalControlLockOut = false;
int NodeId;
#define DaughterBoardSense 2
#define BatteryVoltagePin 4
float LastBatteryVoltage;

//Zone 
#define Zone1Input 36
#define Zone1Output 27

#define Zone2Input 39
#define Zone2Output 14

#define Zone3Input 34
#define Zone3Output 12

#define Zone4Input 35
#define Zone4Output 23


void setup() {
  Serial.begin(115200);
  Serial.println("Starting...");
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

  preferences.begin("credentials", false);
  Serial.print("Connecting to ");
  Serial.println(preferences.getString("ssid"));
  WiFi.begin(preferences.getString("ssid").c_str(), preferences.getString("password").c_str());
  preferences.end();
  int StartTime = millis();
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  
  if (WiFi.status() == WL_CONNECTED) {
    preferences.begin("SystemSettings", true);
    //set up the MQTT 
    //mqttClient.setServer(preferences.getString("MQTTIP").c_str(), preferences.getUShort("MQTTPORT"));
    mqttClient.setServer(mqttServer,mqttPort);
    mqttClient.setCallback(callback);
    //setup other System Level settings
    LocalControlLockOut = preferences.getBool("LocalLockOut");
    NodeId = preferences.getBool("NodeId");
    Name = Name + String(NodeId);
    preferences.end();
  }
  else {
    //Turn on Bluetooth and put Wifi into AP mode
  }
  
  MqttConnectionCheck();
  ReadVoltage();
}


void SetupAllStoredInformation() {
  /*
   * Comment out the stuff you don't need to update in perferences. 
   */
  preferences.begin("credentials", false);
  //preferences.putString("ssid", "Your WiFi SSID");
  //preferences.putString("password", "Your Wifi Password");
  preferences.end();

  preferences.begin("SystemSettings", false);
  //preferences.putBool("LocalLockOut",false);
  //preferences.putInt("NodeId",0);
  //preferences.putString("MQTTIP", "");
  //preferences.putUShort("MQTTPORT", 1883);
  preferences.end();
}

void loop() {
  MqttConnectionCheck();

  /* Enable/Disable the local input Interrupts
      If they are disabled they will be polled and pushed to MQTT for state
  */
  if (LocalControlLockOut == false) {
    interrupts();
  } else {
    noInterrupts();
    //Read all the inputs and post to MQTT
    for (int x = 1; x < 5; x++){
        String PathName = "/" + Name + "/ZoneInput/" + String(x);
        Serial.println(PathName);
        mqttClient.publish(PathName.c_str(),String(ReadInput(x)).c_str());
      } 
  }

//  delay(250);
//  mqttClient.publish("/MakeItRain0/ZoneOutput/4","1");
//  delay(250);
//  mqttClient.publish("/MakeItRain0/ZoneOutput/4","0");

  long CurrentTime = millis();
  if (abs(ThirtyMinTimer - CurrentTime) > 180000) {
    ReadVoltage();
  }

}

void MqttConnectionCheck() {
  if (!mqttClient.connected()){
    reconnect();
    mqttClient.loop();
  }
  
}

void ReadVoltage() {
  LastBatteryVoltage = round((30.954 / 4095) * analogRead(BatteryVoltagePin));
  mqttClient.publish("Votlage", String(LastBatteryVoltage).c_str());
}

void LocalInput1() {
  if (digitalRead(Zone1Output) == LOW) {
    SetOutput(1, HIGH);
  }
  else {
    SetOutput(1, LOW);
  }
  String PathName = "/" + Name + "/ZoneOutput/1";
  mqttClient.publish(PathName.c_str(),String(ReadOutput(1)).c_str());
       
}

void LocalInput2() {
  if (digitalRead(Zone2Output) == false) {
    SetOutput(2, HIGH);
  }
  else {
    SetOutput(2, LOW);
  }
  String PathName = "/" + Name + "/ZoneOutput/2";
  mqttClient.publish(PathName.c_str(),String(ReadOutput(2)).c_str());
}

void LocalInput3() {
  if (digitalRead(Zone3Output) == false) {
    SetOutput(3, HIGH);
  }
  else {
    SetOutput(3, LOW);
  }
  String PathName = "/" + Name + "/ZoneOutput/3";
  mqttClient.publish(PathName.c_str(),String(ReadOutput(3)).c_str());
}

void LocalInput4() {
  if (digitalRead(Zone4Output) == false) {
    SetOutput(4, HIGH);
  }
  else {
    SetOutput(4, LOW);
  }
  String PathName = "/" + Name + "/ZoneOutput/4";
  mqttClient.publish(PathName.c_str(),String(ReadOutput(4)).c_str());
}

void SetOutput(int Number, bool State) {
  /*

  */
  switch (Number) {
    case 1:
      digitalWrite(Zone1Output, State);
      break;
    case 2:
      digitalWrite(Zone2Output, State);
      break;
    case 3:
      digitalWrite(Zone3Output, State);
      break;
    case 4:
      digitalWrite(Zone4Output, State);
      break;
  }


}

bool ReadOutput(int Number) {
  /*

  */
  switch (Number) {
    case 1:
      return digitalRead(Zone1Output);
      break;
    case 2:
      return digitalRead(Zone2Output);
      break;
    case 3:
      return digitalRead(Zone3Output);
      break;
    case 4:
      return digitalRead(Zone4Output);
      break;
  }
}

bool ReadInput(int Number) {
  /*

  */
  switch (Number) {
    case 1:
      return digitalRead(Zone1Input);
      break;
    case 2:
      return digitalRead(Zone2Input);
      break;
    case 3:
      return digitalRead(Zone3Input);
      break;
    case 4:
      return digitalRead(Zone4Input);
      break;
  }
}

void reconnect() {
  // Loop until we're reconnected
  while (!mqttClient.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (mqttClient.connect(Name.c_str())) {
      Serial.println("connected");
      for (int x = 1; x < 5; x++){
        String PathName = "/" + Name + "/ZoneOutput/" + String(x);
        Serial.println(PathName);
        mqttClient.subscribe(PathName.c_str());
        mqttClient.publish(PathName.c_str(),String(ReadOutput(x)).c_str());
      } 
    } else {
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
  for (int i=0;i<length;i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
}
