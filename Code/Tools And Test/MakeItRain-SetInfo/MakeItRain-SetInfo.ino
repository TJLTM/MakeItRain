#include <Preferences.h>

Preferences preferences;

void setup() {
  Serial.begin(115200);
  Serial.println("Putting stuff into memory");

  preferences.begin("credentials", false);
  preferences.clear();
  preferences.putString("ssid", "");
  preferences.putString("ssid_password", "");
  preferences.putString("Admin_password", "SoOriginalThereBoss");
  preferences.end();

  preferences.begin("SystemSettings", false);
  preferences.putBool("LocalLockOut", true);
  preferences.putBool("Battery", false);
  preferences.putString("MQTTIP", ""); //Tested with IP not hostnames
  preferences.putInt("MQTTPORT", 1883);
  preferences.putString("APMode_Password", "MUNAAAYE");
  preferences.putBool("EnableMQTT", true);
  preferences.putBool("EnableWIFI", true);
  preferences.putBool("APMode", true);
  preferences.putFloat("Z1_Max", 7.5);
  preferences.putFloat("Z2_Max", 7.5);
  preferences.putFloat("Z3_Max", 7.5);
  preferences.putFloat("Z4_Max", 7.5);
  preferences.putFloat("Z5_Max", 7.5);
  preferences.putFloat("Z6_Max", 7.5);
  preferences.putFloat("Z7_Max", 7.5);
  preferences.putFloat("Z8_Max", 7.5);
  preferences.putString("ID", "");
  preferences.putBool("IDOverride", false);
  preferences.end();

  preferences.begin("Network_Settings", false);
  preferences.putBool("DHCP", true);
  preferences.putString("GATEWAY", "");
  preferences.putString("IP", "");
  preferences.putString("DNS", "");
  preferences.putString("Subnet", "");
  preferences.putString("NTP", "");
  preferences.end();

  Serial.println("Done");

}


void loop() {
}



void ClearAllStoredData() {
  preferences.begin("credentials", false);
  preferences.clear();
  preferences.end();
  preferences.begin("SystemSettings", false);
  preferences.clear();
  preferences.end();
}
