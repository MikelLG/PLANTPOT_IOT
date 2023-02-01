

#include <Arduino.h>
//#include <DNSServer.h>
#include <WiFiManager.h>

//#include <WiFiUdp.h>
#include "ArduinoOTA.h"
//
#include <PubSubClient.h>
#include <ESP8266WiFi.h>
//#include <ESP8266WebServer.h>

//const char* wifi_ssid = "MIWIFI_2G_299x";
//const char* wifi_password = "ewr2t5y274ym";
char* mqtt_server = "192.168.1.130";

const char* host = "PlugWithRelay";
//char* mqtt_user = "";
//char* mqtt_password = "";


int RELAYPIN = 0;


/// THE RELAY PIN ON THE ESP IS THE PIN 0
int relayOn = 0;
// ------------------

String relay_str;
char relay[50];

long lastMsg = 0;

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

  if (String(topic) == "system/relay01/switch"){
     //ESTO ENCIENDE O APAGA EL LED

    if(messageTemp == "on"){
      Serial.println("relay on");
      digitalWrite(RELAYPIN, 1);
      relayOn = 1;
      digitalWrite(1, 0);
    }
    else if(messageTemp == "off"){
      Serial.println("relay off");
      digitalWrite(RELAYPIN, 0);
      relayOn = 0;
      digitalWrite(1, 1);//ESTO ENCIENDE O APAGA EL LED
    }
  }

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
    if (mqtt_client.connect("ESP8266RELAYClient")) {
      Serial.println("connected");  
      // Subscribe or resubscribe to a topic
      // You can subscribe to more topics (to control more LEDs in this example)

      mqtt_client.subscribe("room/lamp");

      if (mqtt_client.subscribe("system/relay01/switch")) {
        Serial.println("subcribe system/relay01/switch ok");
      } else {
        Serial.println("subcribe system/relay01/switch fail");
      }
    }else {
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

  relayOn = 0;
  pinMode(1, OUTPUT);

  ///--------------------------



  Serial.println("Booting");
  pinMode(BUILTIN_LED, OUTPUT);     // Initialize the BUILTIN_LED pin as an output
  //Serial.setTimeout(2000);
  //
  pinMode(RELAYPIN, OUTPUT);


  //-------------------
  WiFiManager wifiManager;

  wifiManager.autoConnect("APNAME");

  Serial.println("connected...yeey :)");

  WiFi.hostname("RELAY_CONTROL");
 


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
  

  //web_setup();
}


void loop() {
  
  ArduinoOTA.handle();

  if (!mqtt_client.connected()) {
    reconnect();
  }
  
  mqtt_client.loop();

  relay_str = String(relayOn);
  relay_str.toCharArray(relay, relay_str.length() + 1);
  
  long now = millis();
  if (now - lastMsg > 1000){
    lastMsg = now;
    //char msg = relayOn;
    mqtt_client.publish("system/relay01/state", relay);
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


