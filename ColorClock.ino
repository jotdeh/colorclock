// *** SERVER CODE: https://tttapa.github.io/ESP8266/Chap10%20-%20Simple%20Web%20Server.html
// modification to get mdns to work, in loop(): MDNS.update();   

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WiFiMulti.h> 
#include <ESP8266mDNS.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>

ESP8266WebServer server(80);    // Create a webserver object that listens for HTTP request on port 80
IPAddress ip(192, 168, 1, 198); // if you want an ip address automatically assigned, comment the WiFi.config() line in setup()
IPAddress gateway(192,168,1,1);
IPAddress subnet(255,255,255,0);
IPAddress dns(8,8,8,8);
const char* ssid = "Your WiFi Name"; // you only need to change these if you don't want to setup WiFi connection with the web interface
const char* password = "Your WiFi Password";



void handleRoot();              // function prototypes for HTTP handlers
void handleLogin();
void handleNotFound();



// parameters
struct SettingsStruct {
  long backgroundcolor;
  long hourcolor;
  long minutecolor;
  long secondcolor;
  
  bool displayseconds;
  
  byte normalbrightness;
  char dim;
  byte dimmedbrightness;
  byte dimstarthour;
  byte dimstartminute;
  byte dimstophour;
  byte dimstopminute;
} settings, settings_default;

bool demomode;

const char DIMON = '1';
const char DIMOFF = '0';
const char DIMSCHEDULED = '2';

void connect_wifi(){
  // WiFiManager
  // Local intialization. Once its business is done, there is no need to keep it around
  WiFiManager wifiManager;

  // fetches ssid and pass from eeprom and tries to connect
  // if it does not connect it starts an access point with the specified name
  // here  "ColorClock_AP"
  // and goes into a blocking loop awaiting configuration.
  // It will stop this AP when it has established an internet connection. 
  wifiManager.autoConnect("ColorClock_AP", "12345678");

  
  boolean result = WiFi.softAP("ColorClock_AP", "12345678"); // password must be at least 8 characters.
  WiFi.persistent(true);
  //WiFi.mode(WIFI_STA); 
  //WiFi.config(ip,gateway,subnet,dns); // uncomment if you want to use a static IP
  //WiFi.begin(ssid,password);

  Serial.println("Connecting ...");
  int i = 0;
  while (WiFi.status() != WL_CONNECTED) { // Wait for the Wi-Fi to connect: scan for Wi-Fi networks, and connect to the strongest of the networks above
    delay(250);
    Serial.print('.');
  }
  Serial.println('\n');
  Serial.print("Connected to ");
  Serial.println(WiFi.SSID());               // Tell us what network we're connected to
  Serial.print("IP address:\t");
  Serial.println(WiFi.localIP());            // Send the IP address of the ESP8266 to the computer
  Serial.print("Gateway: \t");
  Serial.println(WiFi.gatewayIP());
  Serial.print("Subnet: \t");
  Serial.println(WiFi.subnetMask());
  Serial.print("DNS: \t");
  Serial.println(WiFi.dnsIP());
}


void setup(void){
  Serial.begin(115200);         // Start the Serial communication to send messages to the computer
  delay(10);
  Serial.println('\n');

  // *** Settings 
  settings_default.backgroundcolor = 0x00ff00;
  settings_default.hourcolor = 0xff00ff;
  settings_default.minutecolor = 0xffff00;
  settings_default.secondcolor = 0x0000ff;
  settings_default.displayseconds = true;
  
  settings_default.normalbrightness = 255;
  settings_default.dim = DIMSCHEDULED; 
  settings_default.dimmedbrightness = 64;
  settings_default.dimstarthour = 19;
  settings_default.dimstartminute = 30;
  settings_default.dimstophour = 7;
  settings_default.dimstopminute = 45;

  demomode = 0;

  setup_led();
  
  connect_wifi();
  setup_time();
  setup_server();
  
  
  setup_eeprom();
  
  
}

void loop(void){
//  Serial.println("loop");
  loop_server();
//  showTime();
  loop_time();
  loop_led();
}
