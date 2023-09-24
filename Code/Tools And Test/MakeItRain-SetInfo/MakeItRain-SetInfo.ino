#include <Preferences.h>

Preferences preferences;

void setup() {
  Serial.begin(115200);
  Serial.println("Putting stuff into memory");

  preferences.begin("credentials", false);
  preferences.clear();
  preferences.putString("ssid", "...");
  preferences.putString("ssid_password", "...");
  preferences.putString("Admin_password", "SoOriginalThereBoss");
  preferences.end();

  preferences.begin("SystemSettings", false);
  preferences.clear();
  preferences.putBool("LocalControlToggle", true);
  preferences.putBool("Battery", false);
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
  preferences.putFloat("Z5_Max", 7.5);
  preferences.putFloat("Z6_Max", 7.5);
  preferences.putFloat("Z7_Max", 7.5);
  preferences.putFloat("Z8_Max", 7.5);
  preferences.putString("ID", "");
  preferences.putBool("IDOverride", false);
  preferences.end();

  preferences.begin("Network_Settings", false);
  preferences.clear();
  preferences.putBool("DHCP", true);
  preferences.putString("GATEWAY", "");
  preferences.putString("IP", "");
  preferences.putString("DNS", "");
  preferences.putString("Subnet", "");
  preferences.end();

  preferences.begin("NTP_Settings", false);
  preferences.clear();
  preferences.putBool("NTP_Enable", true);
  preferences.putString("NTP_Server1", "time.nist.gov");
  preferences.putString("NTP_Server2", "pool.ntp.org");
  preferences.putInt("GMTOffset_Sec", 0);
  preferences.putInt("DSTOffset_sec", 3600);
  preferences.putString("TZ", "PST8PDT,M3.2.0,M11.1.0");
  preferences.end();


  preferences.begin("Schedule_Settings", false);
  preferences.clear();
  preferences.putBool("Schedule_Enable", true);
  preferences.putString("S1_Zones", ""); // 1 2 3 4 5 6 7 8
  preferences.putString("S1_Days", ""); // M Tu W Th F Sa Su
  preferences.putString("S1_Duration", ""); //Seconds
  
  preferences.putString("S2_Zones", "");
  preferences.putString("S2_Days", "");
  preferences.putString("S2_Duration", "");
  
  preferences.putString("S3_Zones", "");
  preferences.putString("S3_Days", "");
  preferences.putString("S3_Duration", "");


  
  
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
