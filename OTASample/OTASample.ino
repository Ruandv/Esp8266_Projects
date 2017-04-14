/*
1. Get a Blynk Account
2. Download the Blynk App from your AppStore
3. Set auth variable with that of your project
4. Add a Terminal hooked to virtual Pin 2 (V2)
5. Add a Guage that will show you the Signal Strength (V0) minVal=  0 MaxVal =-100
6. Add a RTC Widget (Real Time Clock) hook to V15
7. Start the project and Enjoy
*/

#include <ESP8266WiFi.h>       
#include <WiFiManager.h>         
#include <ESP8266HTTPUpdateServer.h>    
#include <BlynkSimpleEsp8266.h>
#include <WidgetRTC.h> 
#include <ESP8266mDNS.h>
#include <EEPROM.h>

ESP8266WebServer httpServer(80);
ESP8266HTTPUpdateServer httpUpdater;

WidgetRTC rtc;
BLYNK_ATTACH_WIDGET(rtc, V15);
WidgetTerminal terminal(V2);
 
char auth[] = "78c13d0a5fbf490fa4ff6aa2e9ffe62d";
const char* host = "MyCoolESP";
const char* ssid = "";
const char* passphrase = "";
bool isConnected = false;
const String Version = "2.36";
 

void setup() {
  Serial.begin(115200);
  SetPinModes();
  SetupWiFi();
  SetupOTA();
  SetupConnectionToBlynk();
}

void loop() {
  httpServer.handleClient();
  Blynk.run();
}

void SetPinModes(){
  pinMode(12,OUTPUT);
  digitalWrite(12,LOW); 
}

void SetupOTA(){
  MDNS.begin(host);
  httpUpdater.setup(&httpServer);
  httpServer.begin();
  MDNS.addService("http", "tcp", 80);
  while(WiFi.status()!= WL_CONNECTED){
    WriteMessage(".");
    delay(200);
    WriteMessage("X");
    delay(200);
  }
}
 
void SetupWiFi(){
  WriteMessage("Set WiFi");
  char* pass =GetWiFiPassword();
  char* ssid =  GetSSID();
}
void SetupConnectionToBlynk()
{
  WriteMessage("Connecting to Blynk!");
  Blynk.begin(auth, ssid, passphrase);
}

BLYNK_CONNECTED() {
  WriteMessage("Blynk Connected");
  if(!isConnected)
  {
    isConnected = true;
    WriteMessage("Sync All");
    Blynk.syncAll();
  }
  else
  {
    WriteMessage("No Sync");
  }
  GetInfo();
  rtc.begin();
}

void WriteMessage(String msg)
{
  String currentTime = String(hour()) + ":" + minute() + ":" + second();
  String currentDate = String(day()) + " " + month() + " " + year();
  msg = currentTime + " - " + msg; 
  if (isConnected)
  {
    terminal.println(msg);
    terminal.flush();
  }
  Serial.println(msg);
}

char* GetWiFiPassword(){
  WriteMessage("Reading EEPROM pass");
  String epass = "";
  for (int i = 32; i < 96; ++i)
    {
      epass += char(EEPROM.read(i));
    }
  WriteMessage("PASS: ");
  WriteMessage(epass);  
  return convertToCharArr(epass);
}

char* convertToCharArr(String value){
  char buff[value.length()+1];
  value.toCharArray(buff, value.length()+1) ;
  return buff;
}

char*  GetSSID(){
  EEPROM.begin(512);
  delay(10);
  WriteMessage("\r\n\r\nStartup");
  WriteMessage("Reading EEPROM ssid");
  String esid;
  for (int i = 0; i < 32; ++i)
    {
      esid += char(EEPROM.read(i));
    }
  WriteMessage("SSID: " + esid);
  
  return convertToCharArr(esid);
}


BLYNK_READ(V0) // Widget in the app READs Virtal Pin V5 with the certain frequency
{
  long rssi = WiFi.RSSI();
  Blynk.virtualWrite(0,rssi);
  WriteMessage(".");
}

BLYNK_WRITE(V2) //Terminal
{
  if(String("GetInfo") == param.asStr() or String("getinfo") == param.asStr() or String("Getinfo") == param.asStr())
  {
    GetInfo();
  }
  else
  {
    WriteMessage("I dont understand your command " + String(param.asStr()));
  }
}
  

void GetInfo(){
  WriteMessage("T \r\n\r\n\r\n\r\n\r\n\r\n\r\n\r\n\r\n ");
  WriteMessage("\r\nConnected To : " + WiFi.SSID() + 
               "\r\nSignal :" + String(WiFi.RSSI()) +
               "\r\nVersion : " + Version +
               "\r\nIp Address : " +  WiFi.localIP());
}

