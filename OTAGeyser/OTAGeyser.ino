//#define BLYNK_PRINT Serial    // Comment this out to disable prints and save space
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
WidgetTerminal terminal(V2);

char auth[] = "";
const char* host = "MyCoolESP";
const char* ssid = "";
const char* pass = "";

bool isConnected = false;
const String Version = "2.45";
int geyserPin  = 12;

void setup() {
  Serial.begin(115200);
  //ssid = "Kransberg Turn office 2";
  ssid = "";
  pass = "";
  
  SetPinModes();
  SetupOTA();
  SetupConnectionToBlynk();
}

void loop() {

    Blynk.run();
    httpServer.handleClient();
}

void SetPinModes() {
  pinMode(geyserPin, OUTPUT);
  digitalWrite(geyserPin, HIGH);   
}

void SetupOTA() {
  MDNS.begin(host);
  httpUpdater.setup(&httpServer);
  httpServer.begin();
  MDNS.addService("http", "tcp", 80);
}

void SetupConnectionToBlynk()
{
    WriteMessage("Connecting to Blynk!");
    Blynk.begin(auth, ssid, pass);
    while ( ! Blynk.connected() )
    {
      delay(500);      
      WriteMessage("waiting...");
      delay(500);
    }    
}

BLYNK_CONNECTED() {
  WriteMessage("Blynk Connected");
  if (!isConnected)
  {
    isConnected = true;
    WriteMessage("Sync All");
    Blynk.syncAll();
  }
  else
  {
    WriteMessage("No Sync");
  }
  rtc.begin();
  GetInfo();  
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

char* convertToCharArr(String value) {
  char buff[value.length() + 1];
  value.toCharArray(buff, value.length() + 1) ;
  return buff;
}

BLYNK_READ(V0) // Widget in the app READs Virtal Pin V5 with the certain frequency
{
  long rssi = WiFi.RSSI();
  Blynk.virtualWrite(0, rssi);
}

BLYNK_WRITE(V3) ///Geyser Out
{
  int value = param.asInt();
  WriteMessage(String(value));
  digitalWrite(geyserPin, value);
  printGeyserStatus();
}

BLYNK_WRITE(V2)
{
  if (String("GetInfo") == param.asStr() or String("getinfo") == param.asStr() or String("Getinfo") == param.asStr())
  {
    GetInfo();
  }
  else
  {
    WriteMessage("I dont understand your command " + String(param.asStr()));
  }
}

void GetInfo() {
  WriteMessage("T \r\n\r\n\r\n\r\n\r\n\r\n\r\n\r\n\r\n ");
  WriteMessage("\r\nConnected To : " + WiFi.SSID() +
               "\r\nSignal :" + String(WiFi.RSSI()) +
               "\r\nVersion : " + Version +
               "\r\nIp Address : " +  WiFi.localIP().toString() );
  printGeyserStatus();
}

void printGeyserStatus() {
  String myStatus = "On";
  if (digitalRead(geyserPin) == HIGH) {
    myStatus = "Off";
  }
  
  WriteMessage("Geyser Status : " + myStatus );
}

