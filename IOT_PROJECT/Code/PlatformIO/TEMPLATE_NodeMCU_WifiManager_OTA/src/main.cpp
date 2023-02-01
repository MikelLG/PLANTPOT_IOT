#include <Arduino.h>
#include <ESP8266WiFi.h>          //https://github.com/esp8266/Arduino

#include <DNSServer.h>

//#include <ESP8266WebServer.h>
#include <WiFiManager.h>         //https://github.com/tzapu/WiFiManager

//#include <ESP8266mDNS.h>
#include <WiFiUdp.h>

#include <ArduinoOTA.h>    //https://github.com/esp8266/Arduino/tree/master/libraries/ArduinoOTA


#include <PubSubClient.h>   //MQTT COMUNICATION

/*
  Basic ESP8266 MQTT example

  This sketch demonstrates the capabilities of the pubsub library in combination
  with the ESP8266 board/library.

  It connects to an MQTT server then:
    - publishes "hello world" to the topic "outTopic" every two seconds
    - subscribes to the topic "inTopic", printing out any messages
      it receives. NB - it assumes the received payloads are strings not binary
    - If the first character of the topic "inTopic" is an 1, switch ON the ESP Led,
      else switch it off

      

  It will reconnect to the server if the connection is lost using a blocking
  reconnect function. See the 'mqtt_reconnect_nonblocking' example for how to
  achieve the same result without blocking the main loop.

  To install the ESP8266 board, (using Arduino 1.6.4+):
    - Add the following 3rd party board manager under "File -> Preferences -> Additional Boards Manager URLs":
        http://arduino.esp8266.com/stable/package_esp8266com_index.json
    - Open the "Tools -> Board -> Board Manager" and click install for the ESP8266"
    - Select your ESP8266 in "Tools -> Board"

  */


//Define what IP have the mqtt BROKER

const char* mqtt_server = "192.168.1.130";

String ex1_str;
String ex2_str;
char ex1[50];
char ex2[50];

WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
char msg[50];
long value = 0;

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (unsigned int i = 0; i < length; i++) {
    Serial.print((char)payload[i]); 
  }
  Serial.println();

  // Switch on the LED if an 1 was received as first character
  if ((char)payload[0] == '1') {
    digitalWrite(LED_BUILTIN, LOW);   // Turn the LED on (Note that LOW is the voltage level
    // but actually the LED is on; this is because
    // it is active low on the ESP-01)
  } else {
    digitalWrite(LED_BUILTIN, HIGH);  // Turn the LED off by making the voltage HIGH
  }

}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP8266Client-RandomID";  //Change <---
    //clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str())) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      //client.publish("outTopic", "hello world");
      client.publish("example/outtopic", "Enviando el primer mensaje");
      // ... and resubscribe
      //client.subscribe("inTopic");
      client.subscribe("intopic");
      
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

 
void setup() {
  Serial.begin(115200);
  Serial.println("Booting");
  pinMode(LED_BUILTIN, OUTPUT);     // Initialize the BUILTIN_LED pin as an output
  Serial.setTimeout(2000);
  //setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);

  //ArduinoOTA.setPort(8266);


  //WiFiManager
      //Local intialization. Once its business is done, there is no need to keep it around
      WiFiManager wifiManager;

      //reset saved settings
    //wifiManager.resetSettings();

    //set custom ip for portal
    //wifiManager.setAPConfig(IPAddress(10,0,1,1), IPAddress(10,0,1,1), IPAddress(255,255,255,0));

    //fetches ssid and pass from eeprom and tries to connect
    //if it does not connect it starts an access point with the specified name
    //here  "AutoConnectAP"
    //and goes into a blocking loop awaiting configuration
    //if you like you can create AP with password
    //wifiManager.autoConnect("APNAME", "password");
    wifiManager.autoConnect("DHT22ESP");
    //or use this for auto generated name ESP + ChipID
    //wifiManager.autoConnect();
    WiFi.hostname("DHT22");
    //if you get here you have connected to the WiFi
    Serial.println("connected...yeey :)");

  // Port defaults to 8266
   //ArduinoOTA.setPort(8266);

  // Hostname defaults to esp8266-[ChipID]
   //ArduinoOTA.setHostname("DHT22ESP");

  // No authentication by default
  // ArduinoOTA.setPassword("admin");

  // Password can be set with it's md5 value as well
  // MD5(admin) = 21232f297a57a5a743894a0e4a801fc3
  // ArduinoOTA.setPasswordHash("21232f297a57a5a743894a0e4a801fc3");



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


}

void getAndSendData(){
  Serial.println("Collecting data.");

  //HERE WE SHOULD BE RECIEVING DATA
  float dataExample1 = 12;
  float dataExample2 = 20;

  // Just debug messages
  Serial.print( "Sending data : [" );
  Serial.print( dataExample1 );
  Serial.print( "," );
  Serial.print( dataExample2 );
  Serial.print( "]   -> " );

  // Prepare a JSON payload string
  String payload = "{";
  payload += "\"dataExample1\":"; payload += dataExample1; payload += ",";
  payload += "\"dataExample2\":"; payload += dataExample2;
  payload += "}";

  // Send payload of the JSON
  char attributes[100];
  payload.toCharArray( attributes, 100 );
  client.publish( "dataExample/topic", attributes );
  Serial.println( attributes );

  

  ex1_str = String(dataExample1); //converting temp (the float variable above) to a string 
  ex1_str.toCharArray(ex1, ex1_str.length() + 1); //packaging up the data to publish to mqtt whoa...

  ex2_str = String(dataExample2); //converting Humidity (the float variable above) to a string
  ex2_str.toCharArray(ex2, ex2_str.length() + 1); //packaging up the data to publish to mqtt whoa...


  
    client.publish("exampletopic/dataExample1", ex1); //money shot
    client.publish("exampletopic/dataExample2", ex2);
}



void loop() {
   //Here is to be able to run updates by OTA (On air Updates)
  ArduinoOTA.handle();

  //any custom code or logic goes here

  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  long now = millis();
  if (now - lastMsg > 2000) {  //This is the 2 seconds delay to send message every 2 seconds.
    lastMsg = now;
    ++value;
    snprintf (msg, 50, "hello world #%ld", value);
    Serial.print("Publish message: ");
    Serial.println(msg);

    //client.publish("outTopic", msg);
    //client.publish("casa/despacho/temperatura", msg);

    getAndSendData();
    
  }
}
