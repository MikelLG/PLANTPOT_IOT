

#include <Arduino.h>
//#include <DNSServer.h>
#include <WiFiManager.h>

//#include <WiFiUdp.h>
#include "ArduinoOTA.h"
//
#include <PubSubClient.h>
#include <ESP8266WiFi.h>
//#include <ESP8266WebServer.h>

#include <images.h>

// THIS LIBRARIES ARE NEEDED FOR USING THE TFT screen

#include <Adafruit_GFX.h>    // Core graphics library
//#include <XTronical_ST7735.h> // Hardware-specific library
#include <Adafruit_ST7735.h>   // include Adafruit ST7735 TFT library
#include <SPI.h>
 
// set up pins we are going to use to talk to the screen
#define TFT_RST   D1 //D4 before     // TFT RST pin is connected to NodeMCU pin D4 (GPIO2)
#define TFT_CS    D3     // TFT CS  pin is connected to NodeMCU pin D4 (GPIO0)
#define TFT_DC    D2     // TFT DC  pin is connected to NodeMCU pin D4 (GPIO4)
 
Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS,  TFT_DC, TFT_RST); 

int caca = 1;

String pressurePSI = String(0);
String relayPump;
String solenoid;
String systemEnabled;
String maxpressure = String(0);
String minpressure = String(0);

String ipAddress;

String oldpressurePSI = String(0);
String oldmaxpressure = String(0);
String oldminpressure = String(0);



int led = D4;

//const char* wifi_ssid = "MIWIFI_2G_299x";
//const char* wifi_password = "ewr2t5y274ym";
//char* mqtt_server = "192.168.4.1";
const char* mqtt_server = "192.168.4.1";


const char* host = "ESP_TFT_SCREEN";
//char* mqtt_user = "";
//char* mqtt_password = "";

//Using millis instead of delay
int period = 1000;
int period2 = 2000; 
unsigned long time_now = 0;


 int RELAYPIN = 0;  //    DISABLED BECAUSE NOT USING A RELAY IN THIS NODEMCU 


/// THE RELAY PIN ON THE ESP IS THE PIN 0
int relayOn = 0;
// ------------------

String relay_str;
char relay[50];

String caca_str;
char cacachar[50];

//WiFi.hostname("RELAY_CONTROL");

// ------------------

/*
long lastMsg = 0;
char msg[50];
int value = 0;
*/
/// --------------------- 


void mqtt_callback(char* topic, byte* payload, unsigned int len) {
  
  String tmp=topic;

  Serial.print("Message arrived on topic: ");
  Serial.println(tmp);
  Serial.print(". Message: ");
  String messageTemp;

  for (int i = 0; i < len; i++) {
    Serial.print((char)payload[i]);
    messageTemp += (char)payload[i];
  }
  Serial.println();

//room/relay01/on

//This is what switches ON the Solenoid by activating the relay

if(tmp.indexOf("system/brain/pressuresensor/psi")>=0){
  //pressurePSI = tmp.indexOf("system/brain/pressure/psi");
  pressurePSI = messageTemp;
  Serial.print("PSI:");
  Serial.print(pressurePSI);
}

if(tmp.indexOf("system/brain/relaypump")>=0){
  relayPump = messageTemp;
  Serial.print("RelayPump:");
  Serial.print(relayPump);
}

if(tmp.indexOf("system/box1/solenoid/on")>=0){
  solenoid = String(1);
  Serial.print("Solenoid:");
  Serial.print(solenoid);
}
if(tmp.indexOf("system/box1/solenoid/off")>=0){
  solenoid = String(0);
  Serial.print("Solenoid:");
  Serial.print(solenoid);
}
if(tmp.indexOf("system/enabled")>=0){
  systemEnabled = messageTemp;
  Serial.print("System Enabled:");
  Serial.print(systemEnabled);
}
if(tmp.indexOf("system/brain/maxpressure")>=0){
  maxpressure = messageTemp;
  Serial.print("Max Pressure: ");
  Serial.print(maxpressure);
}
if(tmp.indexOf("system/brain/minpressure")>=0){
  minpressure = messageTemp;
  Serial.print("Min Pressure: ");
  Serial.print(minpressure);
}


  // Feel free to add more if statements to control more GPIOs with MQTT

  Serial.println();
  
}

WiFiClient espRClient;
PubSubClient mqtt_client(mqtt_server, 1883, mqtt_callback, espRClient);


void reconnect() {
  // Loop until we're reconnected
  if (!mqtt_client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    /*
     YOU MIGHT NEED TO CHANGE THIS LINE, IF YOU'RE HAVING PROBLEMS WITH MQTT MULTIPLE CONNECTIONS
     To change the ESP device ID, you will have to give a new name to the ESP8266.
     Here's how it looks:
       if (client.connect("ESP8266Client")) {
     You can do it like this:
       if (client.connect("ESP1_Office")) {
     Then, for the other ESP:
       if (client.connect("ESP2_Garage")) {
      That should solve your MQTT multiple connections problem
    */
    if (mqtt_client.connect("ESP_TFT_SCREEN")) {
      Serial.println("connected");  
      // Subscribe or resubscribe to a topic
      // You can subscribe to more topics (to control more LEDs in this example)
      //mqtt_client.subscribe("room/lamp");
      if (mqtt_client.subscribe("system/box1/solenoid/on")) {
        Serial.println("subcribe system/box1/solenoid/on ok");
        
      } else {
        Serial.println("subcribe system/box1/solenoid/on fail");
      }
      if (mqtt_client.subscribe("system/box1/solenoid/off")) {
        Serial.println("subcribe system/box1/solenoid/off ok");
      } else {
        Serial.println("subcribe system/box1/solenoid/off fail");
      }
      if (mqtt_client.subscribe("system/brain/pressuresensor/psi")) {
        Serial.println("subcribe system/brain/pressuresensor/psi ok");
        
      } else {
        Serial.println("subcribe system/brain/pressuresensor/psi fail");
      }
       if (mqtt_client.subscribe("system/brain/relaypump")) {
        Serial.println("subcribe system/brain/relaypump ok");
        
      } else {
        Serial.println("subcribe system/brain/relaypump fail");
      }
       if (mqtt_client.subscribe("system/enabled")) {
        Serial.println("subcribe system/enabled ok");
        
      } else {
        Serial.println("subcribe system/enabled fail");
      }
       if (mqtt_client.subscribe("system/brain/maxpressure")) {
        Serial.println("subcribe system/brain/maxpressure ok");
        
      } else {
        Serial.println("subcribe system/brain/maxpressure fail");
      }
      if (mqtt_client.subscribe("system/brain/minpressure")) {
        Serial.println("subcribe system/brain/minpressure ok");
        
      } else {
        Serial.println("subcribe system/brain/minpressure fail");
      }
     

    }
    else {
      Serial.print("failed, rc=");
      Serial.print(mqtt_client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}


void setup() {
  //
  Serial.begin(115200);
  Serial.println("SerialBegin"); /////////////////////////////////////
  


  tft.initR(INITR_BLACKTAB);   // initialize a ST7735S chip, black tab
  tft.setRotation(0);
  tft.fillScreen(ST7735_WHITE);
  // tft.drawRGBBitmap(56,56,DiskIcon,16,16); 
  tft.drawRGBBitmap(0,16,Roots,128,128);



  /////////// ------------------ HERE SHOULD GO THE CODE TO PROGRAM THE GRAPHICS SCREEN -----------------------------------
  delay(5000);

  tft.fillScreen(ST77XX_BLACK);
  
  tft.fillRect(0,0,64,60,ST77XX_WHITE);         // Draw the backgrounds of each of the spaces on the menu
  tft.fillRect(64,0,64,60,ST77XX_WHITE);
  tft.fillRect(0,60,64,60,ST77XX_WHITE);
  tft.fillRect(64,60,64,60,ST77XX_WHITE);
  tft.fillRect(0,120,128,40,ST77XX_WHITE);


  tft.setCursor(27, 15);  // PRESSURE FROM SENSOR
  tft.setTextSize (2);
  tft.setTextColor ( ST77XX_RED , ST77XX_WHITE);
  tft.print (pressurePSI);                          // Draw the pressure from the sensor as soon as a message is recieved.

  tft.drawRGBBitmap(2,122,Roots36,36,36);           // Draw the small logo into the bottom bar.

  tft.drawRect(0,119,128,2,ST77XX_BLACK);          // Draw the lines to separate the menus
  tft.drawRect(63,0,2,120,ST77XX_BLACK);           
  tft.drawRect(0,59,128,2,ST77XX_BLACK);          
  
  ///     DRAWING THE PSI LETERS

  tft.setCursor(15, 40);
  tft.setTextSize(2);
  tft.setTextColor ( ST77XX_BLACK , ST77XX_WHITE);
  tft.print("PSI");

  ///    DRAWING THE PUMP LETERS
  
  tft.setCursor(75,40);
  tft.setTextSize(2);
  tft.setTextColor ( ST77XX_BLACK , ST77XX_WHITE);
  tft.print("PUMP");

  ///    DRAWING THE SOLENOID LETERS
  
  tft.setCursor(9,98);
  tft.setTextSize(2);
  tft.setTextColor ( ST77XX_BLACK , ST77XX_WHITE);
  tft.print("GATE");


  ///    DRAWING THE MIN MAX PRESSURE LETERS

  tft.setCursor(75,65);
  tft.setTextSize(1);
  tft.setTextColor ( ST77XX_BLACK , ST77XX_WHITE);
  tft.print("Max PSI");

  tft.setCursor(75,92);
  tft.setTextSize(1);
  tft.setTextColor ( ST77XX_BLACK , ST77XX_WHITE);
  tft.print("Min PSI");

  //     DRAWING THE MIN AND MAX PRESSURE VALUES (INIT)


  tft.setCursor(90, 75);  
  tft.setTextSize (2);
  tft.setTextColor ( ST77XX_RED , ST77XX_WHITE);
  tft.print (maxpressure); 

  tft.setCursor(90, 102);  
  tft.setTextSize (2);
  tft.setTextColor ( ST77XX_RED , ST77XX_WHITE);
  tft.print (minpressure); 



 








 

  //
 
  //Serial.write("SerialBegin2");

  relayOn = 0;
 // pinMode(1, OUTPUT);

  ///--------------------------



  Serial.println("Booting");
  // pinMode(BUILTIN_LED, OUTPUT);     // Initialize the BUILTIN_LED pin as an output  ///    ATTENTION!! IF THE BUILTIN_LED IS DEFINED SERIAL MONITOR WILL NOT WORK!
  //Serial.setTimeout(2000);
  
  //pinMode(RELAYPIN, OUTPUT);

  //---------------------
  WiFiManager wifiManager;

  wifiManager.autoConnect("APNAMETFT");

  Serial.println("connected...yeey :)");

  WiFi.hostname("TFT_SCREEN_CONTROL");
 


//////////////////////////////    OTA UPDATE    /////////////////////////////////
  ArduinoOTA.onStart([]() {
    String type;
    if (ArduinoOTA.getCommand() == U_FLASH)
      type = "sketch";
    else // U_SPIFFS
      type = "filesystem";

    // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
    Serial.println("Start updating " + type);
  });
  ArduinoOTA.onEnd([]() {
    Serial.println("\nEnd");
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
    else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
    else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
    else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
    else if (error == OTA_END_ERROR) Serial.println("End Failed");
  });
  ArduinoOTA.begin();
  Serial.println("Ready OTA8");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  
  //------------------------- END OTA UPDATE ---------------------------¿
  mqtt_client.setServer(mqtt_server, 1883);
  mqtt_client.setCallback(mqtt_callback);
  

  //////////////// -------------------------------------------------   IP ADDRESS FEEDBACK ON TFT!   -------------------

  ipAddress = WiFi.localIP().toString();
  //IPaddress =  WiFi.localIP().toString();


  // PRINT THE IP ADDRESS OF THE NODEMCU
  tft.setCursor(45, 125);  // Temperature from DS18B20
  tft.setTextSize (0.5);
  tft.setTextColor ( ST77XX_BLACK , ST77XX_WHITE);
  //tft.print ("IP:");
  tft.print (ipAddress);
  Serial.println("IPPP:");
  Serial.println(ipAddress);
  //ipAddress

   // PRINT THE IP ADDRESS OF THE MQTT BROKER
  tft.setCursor(60, 135);  // Temperature from DS18B20
  tft.setTextSize (0.5);
  tft.setTextColor ( ST77XX_BLACK , ST77XX_WHITE);
  tft.print ("MQTT IP:");
  tft.setCursor(45, 145);
  tft.print (mqtt_server);
  Serial.println("Mqtt Server:");
  Serial.println(ipAddress);
  //ipAddress

  //web_setup();
}


void loop() {
  //Serial.write("1"); /////////////////////////////////////

  ArduinoOTA.handle();

  if (!mqtt_client.connected()) {
    reconnect();
  }
  
  mqtt_client.loop();

  relay_str = String(relayOn);
  relay_str.toCharArray(relay, relay_str.length() + 1);


  if (pressurePSI.toInt() >= 9999){
    pressurePSI = String(9999);
  }

  if(millis() > time_now + period){
    //tft.fillScreen(ST77XX_BLUE);

    if(pressurePSI == oldpressurePSI){

    }
    else {

      /////////// ------------------------------------------ THIS IS DRAWING THE VALUES RECIVED FROM THE MQTT SERVER WITHOUT BLINKING OR OVERPOSING VALUES ---------------------------------

      if (oldpressurePSI.length() == 1){
        tft.setCursor(27, 15);
      }
      else if (oldpressurePSI.length() == 2){
        tft.setCursor(22, 15);
      }
      else if (oldpressurePSI.length() == 3){
        tft.setCursor(16, 15);
      }
      else if (oldpressurePSI.length() == 4){
        tft.setCursor(10, 15);
      }
      else if (oldpressurePSI.length() == 5){
        tft.setCursor(2, 15);
      }
      
      int lenght = pressurePSI.length();
      Serial.print("lenght:");
      Serial.print(lenght);                 
      tft.setTextSize (2);
      tft.setTextColor ( ST77XX_WHITE, ST77XX_WHITE );
      tft.print (oldpressurePSI);
      
      if (pressurePSI.length() == 1){
        tft.setCursor(27, 15);
      }
      else if (pressurePSI.length() == 2){
        tft.setCursor(22, 15);
      }
      else if (pressurePSI.length() == 3){
        tft.setCursor(16, 15);
      }
      else if (pressurePSI.length() == 4){
        tft.setCursor(10, 15);
      }
      else if (pressurePSI.length() == 5){
      tft.setCursor(2, 15);
      }
      //tft.setCursor(2, 15);  // Presure from pressure sensor
      tft.setTextSize (2);
      tft.setTextColor ( ST77XX_RED, ST77XX_WHITE );
      tft.print (pressurePSI);

      oldpressurePSI = pressurePSI;
        ////   ---------------------------------------------------------------------------
    }
    
    /////  -----------------------     THIS WILL SHOW THE STATE OF THE PUMP    --------------------
    if (relayPump == String(1)){

      tft.setCursor(80, 15);  // Pressure from pressure sensor
      tft.setTextSize (2);
      tft.setTextColor ( ST77XX_WHITE, ST77XX_WHITE );
      tft.print("OFF");

      tft.setCursor(85, 15);  // Presure from pressure sensor
      tft.setTextSize (2);
      tft.setTextColor ( ST77XX_GREEN, ST77XX_WHITE );
      tft.print ("ON");
  
    }
    else{

      tft.setCursor(85, 15);  // Pressure from pressure sensor
      tft.setTextSize (2);
      tft.setTextColor ( ST77XX_WHITE, ST77XX_WHITE );
      tft.print("ON");

      tft.setCursor(80, 15);  // Presure from pressure sensor
      tft.setTextSize (2);
      tft.setTextColor ( ST77XX_RED, ST77XX_WHITE );
      tft.print ("OFF");

    }

    if(maxpressure == oldmaxpressure){

    }
    else{

      
      /////////// ------------------------------------------ THIS IS DRAWING THE VALUES RECIVED FROM THE MQTT SERVER WITHOUT BLINKING OR OVERPOSING VALUES ---------------------------------

      tft.setCursor(85, 75);  // Pressure from pressure sensor
      tft.setTextSize (2);
      tft.setTextColor ( ST77XX_WHITE, ST77XX_WHITE );
      tft.print (oldmaxpressure);
      
      tft.setCursor(85, 75);  // Presure from pressure sensor
      tft.setTextSize (2);
      tft.setTextColor ( ST77XX_RED, ST77XX_WHITE );
      tft.print (maxpressure);

      oldmaxpressure = maxpressure;
    }


    if(minpressure == oldminpressure){

    }
    else{

      
      /////////// ------------------------------------------ THIS IS DRAWING THE VALUES RECIVED FROM THE MQTT SERVER WITHOUT BLINKING OR OVERPOSING VALUES ---------------------------------

      tft.setCursor(85, 102);  // Pressure from pressure sensor
      tft.setTextSize (2);
      tft.setTextColor ( ST77XX_WHITE, ST77XX_WHITE );
      tft.print (oldminpressure);
      
      tft.setCursor(85, 102);  // Presure from pressure sensor
      tft.setTextSize (2);
      tft.setTextColor ( ST77XX_RED, ST77XX_WHITE );
      tft.print (minpressure);
      

      oldminpressure = minpressure;
    }

    if (solenoid == String(1)){

      tft.setCursor(9, 75);  // Pressure from pressure sensor
      tft.setTextSize (2);
      tft.setTextColor ( ST77XX_WHITE, ST77XX_WHITE );
      tft.print("OPEN");

      tft.setCursor(2, 75);  // Presure from pressure sensor
      tft.setTextSize (2);
      tft.setTextColor ( ST77XX_GREEN, ST77XX_WHITE );
      tft.print ("CLOSE");
  
    }
    else{

      tft.setCursor(2, 75);  // Pressure from pressure sensor
      tft.setTextSize (2);
      tft.setTextColor ( ST77XX_WHITE, ST77XX_WHITE );
      tft.print("CLOSE");

      tft.setCursor(9, 75);  // Presure from pressure sensor
      tft.setTextSize (2);
      tft.setTextColor ( ST77XX_RED, ST77XX_WHITE );
      tft.print ("OPEN");

    }
      
    

    /*
    tft.setCursor(80, 15);  // Pressure from pressure sensor
    tft.setTextSize (2);
    tft.setTextColor ( ST77XX_WHITE, ST77XX_WHITE );
    tft.print(oldpressurePSI);

    tft.setCursor(80, 15);  // Presure from pressure sensor
    tft.setTextSize (2);
    tft.setTextColor ( ST77XX_RED, ST77XX_WHITE );
    tft.print (pressurePSI);
   */

    
    /*
    tft.setCursor(5, 5);  // Temperature from DS18B20
    tft.setTextSize (2);
    tft.setTextColor ( ST77XX_BLUE, ST77XX_BLUE );
    tft.print (oldpressurePSI);
    
    tft.setCursor(5, 5);  // Temperature from DS18B20
    tft.setTextSize (2);
    tft.setTextColor ( ST77XX_RED, ST77XX_BLUE );
    tft.print (pressurePSI);

    oldpressurePSI = pressurePSI;
    */

  }
  //char msg = relayOn;



  

  //Using millis instead of delay. This will give the state of the solenoid one a second. Period defined on top.
  if(millis() > time_now + period){
        time_now = millis();
        mqtt_client.publish("system/box1/solenoid/state", relay);
        //Serial.print("connected");
        //Serial.write("1");
        digitalWrite(led, LOW);
  }

 
   
  

  
  //delay(3000);
    /*

    if(!mqtt_client.loop())
    mqtt_client.connect("ESP8266Client");

  */

  //mqtt_setup();

  //-----------------------

  

  //-----------------------

  //
  //webserver.handleClient();
  //
 
  
  
}


