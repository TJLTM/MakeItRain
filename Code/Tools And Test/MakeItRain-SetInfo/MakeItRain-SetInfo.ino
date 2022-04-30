#include <Preferences.h>

Preferences preferences;

void setup() {
  Serial.begin(115200);
  Serial.println("Putting stuff into memory");

  preferences.begin("credentials", false);
  preferences.clear();
  preferences.putString("ssid", "...");
  preferences.putString("password", "...!");
  preferences.putString("Admin_password", "SoOriginalThereBoss");
  preferences.end();

  preferences.begin("SystemSettings", false);
  preferences.putBool("LocalLockOut", true);
  preferences.putBool("Battery", true);
  preferences.putString("MQTTIP", ""); //Tested with IP not hostnames
  preferences.putInt("MQTTPORT", 1883);
  preferences.putString("APMode_Password", "MUNAAAYE");
  preferences.putBool("EnableMQTT", false);
  preferences.putBool("EnableWIFI", false);
  preferences.putBool("APMode", true);
  preferences.putFloat("Z1_Max", 7.5);
  preferences.putFloat("Z2_Max", 7.5);
  preferences.putFloat("Z3_Max", 7.5);
  preferences.putFloat("Z4_Max", 7.5);

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
