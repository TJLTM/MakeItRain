#include <PubSubClient.h>
#include <WiFi.h>
#include <Preferences.h>


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
String ZO1Topic = "";

#define Zone2Input 39
#define Zone2Output 14
String ZO2Topic = "";

#define Zone3Input 34
#define Zone3Output 12
String ZO3Topic = "";

#define Zone4Input 35
#define Zone4Output 23
String ZO4Topic = "";


void setup() {
  Serial.begin(115200);
  Serial.println("Starting...");
  SetupAllStoredInformation();
 
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
  int CurrentTime = millis();
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    CurrentTime = millis();
  }

  
  if (WiFi.status() == WL_CONNECTED) {
    preferences.begin("SystemSettings", true);
    //set up the MQTT 
    //mqttClient.setServer(preferences.getString("MQTTIP").c_str(), preferences.getUShort("MQTTPORT"));
    Serial.print("Mqtt port:");
    Serial.println(preferences.getInt("MQTTPORT"));
    mqttClient.setServer(mqttServer,mqttPort);
    mqttClient.setCallback(callback);
    //setup other System Level settings
    LocalControlLockOut = preferences.getBool("LocalLockOut");
    NodeId = preferences.getBool("NodeId");
    Name = Name + String(NodeId);
    preferences.end();
    delay(100);
    MqttConnectionCheck();
  }
  else {
    //Turn on Bluetooth and put Wifi into AP mode
  }
  
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
  preferences.putInt("MQTTPORT", 1883);
  preferences.end();
}

void loop() {
  if (WiFi.status() != WL_CONNECTED){
    Serial.println("Wifi Disconnected do something");
  }
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

  
  long CurrentTime = millis();
  ReadVoltage();
  if (abs(ThirtyMinTimer - CurrentTime) > 180000) {
    String VTopic = "/" + Name + "Votlage";
    mqttClient.publish(VTopic.c_str(), String(LastBatteryVoltage).c_str());
  }

}

void MqttConnectionCheck() {
  if (!mqttClient.connected()){
    reconnect();
  }
  mqttClient.loop();
}

void ReadVoltage() {
  LastBatteryVoltage = round((30.954 / 4095) * analogRead(BatteryVoltagePin));
}

void LocalInput1() {
  if (digitalRead(Zone1Output) == LOW) {
    SetOutput(1, HIGH);
  }
  else {
    SetOutput(1, LOW);
  }
  mqttClient.publish(ZO1Topic.c_str(),String(ReadOutput(1)).c_str());
       
}

void LocalInput2() {
  if (digitalRead(Zone2Output) == false) {
    SetOutput(2, HIGH);
  }
  else {
    SetOutput(2, LOW);
  }
  mqttClient.publish(ZO2Topic.c_str(),String(ReadOutput(2)).c_str());
}

void LocalInput3() {
  if (digitalRead(Zone3Output) == false) {
    SetOutput(3, HIGH);
  }
  else {
    SetOutput(3, LOW);
  }
  mqttClient.publish(ZO3Topic.c_str(),String(ReadOutput(3)).c_str());
}

void LocalInput4() {
  if (digitalRead(Zone4Output) == false) {
    SetOutput(4, HIGH);
  }
  else {
    SetOutput(4, LOW);
  }
  mqttClient.publish(ZO4Topic.c_str(),String(ReadOutput(4)).c_str());
}

void SetOutput(int Number, bool State) {
  /*

  */
  switch (Number) {
    case 1:
      digitalWrite(Zone1Output, State);
      mqttClient.publish(ZO1Topic.c_str(),String(ReadOutput(1)).c_str());
      break;
    case 2:
      digitalWrite(Zone2Output, State);
      mqttClient.publish(ZO2Topic.c_str(),String(ReadOutput(2)).c_str());
      break;
    case 3:
      digitalWrite(Zone3Output, State);
      mqttClient.publish(ZO3Topic.c_str(),String(ReadOutput(3)).c_str());
      break;
    case 4:
      digitalWrite(Zone4Output, State);
      mqttClient.publish(ZO4Topic.c_str(),String(ReadOutput(4)).c_str()); 
      break;
  }
}

String ReadOutput(int Number) {
  /*

  */
  String ValueToReturn = "off";
  switch (Number) {
    case 1:
      if (digitalRead(Zone1Output) == 1){
        ValueToReturn = "on";
      }
      break;
    case 2:
      if (digitalRead(Zone2Output) == 1){
        ValueToReturn = "on";
      }
      break;
    case 3:
      if (digitalRead(Zone3Output) == 1){
        ValueToReturn = "on";
      }
      break;
    case 4:
      if (digitalRead(Zone4Output) == 1){
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
      if (digitalRead(Zone1Input) == 1){
        ValueToReturn = "on";
      }
      break;
    case 2:
      if (digitalRead(Zone2Input) == 1){
        ValueToReturn = "on";
      }
      break;
    case 3:
      if (digitalRead(Zone3Input) == 1){
        ValueToReturn = "on";
      }
      break;
    case 4:
      if (digitalRead(Zone4Input) == 1){
        ValueToReturn = "on";
      }
      break;
  }
  return ValueToReturn;
}

void reconnect() {
  // Loop until we're reconnected
  while (!mqttClient.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (mqttClient.connect(Name.c_str())) {
      Serial.println("connected");
      // sub to the Zone Output topics and pub the currect state 
      ZO1Topic =  "/" + Name + "/ZoneOutput/1";
      mqttClient.subscribe(ZO1Topic.c_str());
      mqttClient.publish(ZO1Topic.c_str(),String(ReadOutput(1)).c_str());
      ZO2Topic =  "/" + Name + "/ZoneOutput/2";
      mqttClient.subscribe(ZO2Topic.c_str());
      mqttClient.publish(ZO2Topic.c_str(),String(ReadOutput(2)).c_str());
      ZO3Topic =  "/" + Name + "/ZoneOutput/3";
      mqttClient.subscribe(ZO3Topic.c_str());
      mqttClient.publish(ZO3Topic.c_str(),String(ReadOutput(3)).c_str());
      ZO4Topic =  "/" + Name + "/ZoneOutput/4";
      mqttClient.subscribe(ZO4Topic.c_str());
      mqttClient.publish(ZO4Topic.c_str(),String(ReadOutput(4)).c_str()); 
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
  for (int i=0;i<length;i++) {
    Serial.print((char)payload[i]);
    message += (char)payload[i];
  }
  Serial.println();
  
  //Zone 1
  if (String((char*)topic) == ZO1Topic){
    CurrentOutputState = String(ReadOutput(1)).c_str();
    if (message != CurrentOutputState){
      if (message == "on"){
        SetOutput(1,true);
        Serial.println("Turning Zone 1: on");
      }
      if (message == "off"){
        SetOutput(1,false);
        Serial.println("Turning Zone 1: off");
      } 
    }
  }

  //Zone 2
  if (String((char*)topic) == ZO2Topic){
    CurrentOutputState = String(ReadOutput(2)).c_str();
    if (message != CurrentOutputState){
      if (message == "on"){
        SetOutput(2,true);
        Serial.println("Turning Zone 2: on");
      }
      if (message == "off"){
        SetOutput(2,false);
        Serial.println("Turning Zone 2: off");
      } 
    }
  }

  //Zone 3
  if (String((char*)topic) == ZO3Topic){
    CurrentOutputState = String(ReadOutput(3)).c_str();
    if (message != CurrentOutputState){
      if (message == "on"){
        SetOutput(3,true);
        Serial.println("Turning Zone 3: on");
      }
      if (message == "off"){
        SetOutput(3,false);
        Serial.println("Turning Zone 3: off");
      } 
    }
  }

  //Zone 4
  if (String((char*)topic) == ZO4Topic){
    CurrentOutputState = String(ReadOutput(4)).c_str();
    if (message != CurrentOutputState){
      if (message == "on"){
        SetOutput(4,true);
        Serial.println("Turning Zone 4: on");
      }
      if (message == "off"){
        SetOutput(4,false);
        Serial.println("Turning Zone 4: off");
      } 
    }
  }

}
