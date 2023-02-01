/*
 *  This sketch sends data via HTTP GET requests to $host.
 *
 *  Magic numbers are used to determine which line(s) to handle, and which part of this line is used.
 *  The numbers are determined using curl (or some other http dump program)
 */
#include <Arduino.h>
#include <ESP8266WiFi.h>          //https://github.com/esp8266/Arduino

#include <DNSServer.h>

//#include <ESP8266WebServer.h>
#include <WiFiManager.h>         //https://github.com/tzapu/WiFiManager

//#include <ESP8266mDNS.h>
#include <WiFiUdp.h>

#include <ArduinoOTA.h>    //https://github.com/esp8266/Arduino/tree/master/libraries/ArduinoOTA

#include <PubSubClient.h>   //MQTT COMUNICATION
#include <ArduinoJson.h> // Arduino JSON for reading .json files

//#include <ESP8266HTTPClient.h>   // If we want to search a http.
#include <WiFiClientSecure.h>  // If we want to search a https.

#ifndef STASSID
#define STASSID "Casa_Anika_2G"
#define STAPSK  "diasfelices"
#endif

//https://openweathermap.org/data/2.5/weather/?appid=439d4b804bc8187953eb36d2a8c26a02&id=3128760&units=metric
// Just the base of the URL you want to connect to
#define TEST_HOST "openweathermap.org"
const int httpsPort = 443; // For http use 80, for https 443


// Use web browser to view and copy
// OPTIONAL - The finferprint of the site you want to connect to.
//#define TEST_HOST_FINGERPRINT "BC 73 A5 9C 6E EE 38 43 A6 37 FC 32 CF 08 16 DC CF F1 5A 66"
String content = "";

//Define what IP have the mqtt BROKER
const char* mqtt_server = "192.168.1.142";

String ex1_str;
String ex2_str;
char ex1[50];
char ex2[50];

/// - -----------------------------------------
// For Non-HTTPS requests
//WiFiClient espClient;

// For HTTPS requests
WiFiClientSecure client;


PubSubClient mqttclient(client);
long lastMsg = 0;
char msg[50];
long value = 0;




float coord_lon;
float coord_lat;
JsonObject weather_0;
int weather_0_id;
const char* weather_0_main;
const char* weather_0_description;
const char* weather_0_icon;
const char* base;
JsonObject main;
float main_temp;
float main_feels_like;
int main_temp_min;
int main_temp_max;
int main_pressure;
int main_humidity;
int visibility;
float wind_speed;
int wind_deg;
int clouds_all;
long dt;
JsonObject sys;
int sys_type;
int sys_id;
const char* sys_country;
long sys_sunrise;
long sys_sunset;
int timezone;
long id;
const char* name;
int cod;

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print(" ");
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
  while (!mqttclient.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP8266Client-RandomID";  //Change <---
    //clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (mqttclient.connect(clientId.c_str())) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      //client.publish("outTopic", "hello world");
      mqttclient.publish("example/outtopic", "Enviando el primer mensaje");
      // ... and resubscribe
      //client.subscribe("inTopic");
      mqttclient.subscribe("intopic");
      
    } else {
      Serial.print("failed, rc=");
      Serial.print(mqttclient.state());
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
  mqttclient.setServer(mqtt_server, 1883);
  mqttclient.setCallback(callback);

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
    wifiManager.autoConnect("WEBSCR-NODEMCU");
    //or use this for auto generated name ESP + ChipID
    //wifiManager.autoConnect();
    WiFi.hostname("WEBSCR-NODEMCU");
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
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  

  // If you don't need to check the fingerprint
  client.setInsecure();

  // If you want to check the fingerprint
  //client.setFingerprint(TEST_HOST_FINGERPRINT);

  //makeHTTPRequest();

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
  mqttclient.publish( "dataExample/topic", attributes );
  Serial.println( attributes );

  

  ex1_str = String(dataExample1); //converting temp (the float variable above) to a string 
  ex1_str.toCharArray(ex1, ex1_str.length() + 1); //packaging up the data to publish to mqtt whoa...

  ex2_str = String(dataExample2); //converting Humidity (the float variable above) to a string
  ex2_str.toCharArray(ex2, ex2_str.length() + 1); //packaging up the data to publish to mqtt whoa...


  
  mqttclient.publish("exampletopic/dataExample1", ex1); //money shot
  mqttclient.publish("exampletopic/dataExample2", ex2);
}


void makeHTTPRequest() {

  // Opening connection to server (Use 80 as port if HTTP)
  if (!client.connect(TEST_HOST, 443))
  {
    Serial.println(F("Connection failed"));
    return;
  }

  // give the esp a breather
  yield();

  // Send HTTP request
  client.print(F("GET "));
  // This is the second half of a request (everything that comes after the base URL)
  client.print("/data/2.5/weather/?appid=439d4b804bc8187953eb36d2a8c26a02&id=3128760&units=metric");
  client.println(F(" HTTP/1.1"));
  
  //www.openweathermap.org/data/2.5/weather/?appid=439d4b804bc8187953eb36d2a8c26a02&id=3128760&units=metric
  //http://www.openweathermap.org/data/2.5/weather/?appid=439d4b804bc8187953eb36d2a8c26a02&id=3128760&units=metric
  //Headers
  client.print(F("Host: "));
  client.println(TEST_HOST);

  client.println(F("Cache-Control: no-cache"));

  if (client.println() == 0)
  {
    Serial.println(F("Failed to send request"));
    return;
  }
  //delay(100);
  // Check HTTP status
  char status[32] = {0};
  client.readBytesUntil('\r', status, sizeof(status));
  if (strcmp(status, "HTTP/1.1 200 OK") != 0)
  {
    Serial.print(F("Unexpected response: "));
    Serial.println(status);
    return;
  }

  // Skip HTTP headers
  char endOfHeaders[] = "\r\n\r\n";
  if (!client.find(endOfHeaders))
  {
    Serial.println(F("Invalid response"));
    return;
  }
  
  // This is probably not needed for most, but I had issues
  // with the Tindie api where sometimes there were random
  // characters coming back before the body of the response.
  // This will cause no hard to leave it in
  // peek() will look at the character, but not take it off the queue
  
  while (client.available() && client.peek() != '{')
  {
    char c = 0;
    client.readBytes(&c, 1);
    Serial.print(c);
    Serial.println("BAD");
  }

  // While the client is still availble read each
  // byte and print to the serial monitor
  while (client.available()) {
    char c = 0;
    client.readBytes(&c, 1);
    //Serial.println("Recieved this:");
    //Serial.print(c);
    
    content.concat(c);
    //Serial.println(content);
  }
  //Serial.println(content);
  
  /////////--------------------------------------------------------------------------------------
  // Parsing
      
      
      //JsonObject& root = jsonBuffer.parseObject(http.getString());




      /*
      // Parameters
      int id = root["id"]; // 1
      const char* name = root["name"]; // "Leanne Graham"
      const char* username = root["username"]; // "Bret"
      const char* email = root["email"]; // "Sincere@april.biz"
      // Output to serial monitor
      Serial.print("Name:");
      Serial.println(name);
      Serial.print("Username:");
      Serial.println(username);
      Serial.print("Email:"); 
      Serial.println(email);
      
    }*/
    //http.end();   //Close connection
  /// ----------------------------------------------------------------------------------------------------

}

/// ------------------------------- DELETE ONE OF BOTH!!!!!!!! ------------------------------------------------------------------------

void httpRequest(String urlRequest){
/*

    //HTTPClient http;  //Object of class HTTPClient
    //http.begin(urlRequest);
    Serial.print(urlRequest);
    //int httpCode = http.GET();
    //Serial.print(httpCode);
    Serial.println("YES");
    //Check the returning code                                                                  
    if (httpCode > 0) {
      // Parsing
      Serial.println("DENTRO");
      const size_t bufferSize = JSON_OBJECT_SIZE(2) + JSON_OBJECT_SIZE(3) + JSON_OBJECT_SIZE(5) + JSON_OBJECT_SIZE(8) + 370;
      //DynamicJsonBuffer jsonBuffer(bufferSize);
      DynamicJsonDocument doc(bufferSize);

      String json = http.getString();
      Serial.println("recievedFile:");
      Serial.print(json);

      deserializeJson(doc, json);
      float coord_lon = doc["coord"]["lon"]; // 2.16
      float coord_lat = doc["coord"]["lat"];

      Serial.println("Coord_lon:");
      Serial.print(coord_lon);
      Serial.println("Coord_lat:");
      Serial.print(coord_lat);
      
      JsonObject& root = jsonBuffer.parseObject(http.getString());




      
      // Parameters
      int id = root["id"]; // 1
      const char* name = root["name"]; // "Leanne Graham"
      const char* username = root["username"]; // "Bret"
      const char* email = root["email"]; // "Sincere@april.biz"
      // Output to serial monitor
      Serial.print("Name:");
      Serial.println(name);
      Serial.print("Username:");
      Serial.println(username);
      Serial.print("Email:"); 
      Serial.println(email);
      
    }
    http.end();   //Close connection
  */
}


void refreshVariables(){

  
  Serial.println("DENTRO");
  const size_t bufferSize = JSON_OBJECT_SIZE(2) + JSON_OBJECT_SIZE(3) + JSON_OBJECT_SIZE(5) + JSON_OBJECT_SIZE(8) + 370;
  //DynamicJsonBuffer jsonBuffer(bufferSize);
  DynamicJsonDocument doc(bufferSize);

  deserializeJson(doc, content);


  coord_lon = doc["coord"]["lon"]; // 2.16
  coord_lat = doc["coord"]["lat"]; // 41.39

  weather_0 = doc["weather"][0];
  weather_0_id = weather_0["id"]; // 801
  weather_0_main = weather_0["main"]; // "Clouds"
  weather_0_description = weather_0["description"]; // "few clouds"
  weather_0_icon = weather_0["icon"]; // "02d"

  base = doc["base"]; // "stations"

  main = doc["main"];
  main_temp = main["temp"]; // 15.4
  main_feels_like = main["feels_like"]; // 11.69
  main_temp_min = main["temp_min"]; // 15
  main_temp_max = main["temp_max"]; // 16
  main_pressure = main["pressure"]; // 1016
  main_humidity = main["humidity"]; // 67

  visibility = doc["visibility"]; // 10000

  wind_speed = doc["wind"]["speed"]; // 5.1
  wind_deg = doc["wind"]["deg"]; // 60

  clouds_all = doc["clouds"]["all"]; // 20

  dt = doc["dt"]; // 1587579013

  sys = doc["sys"];
  sys_type = sys["type"]; // 1
  sys_id = sys["id"]; // 6398
  sys_country = sys["country"]; // "ES"
  sys_sunrise = sys["sunrise"]; // 1587531629
  sys_sunset = sys["sunset"]; // 1587580725

  timezone = doc["timezone"]; // 7200
  id = doc["id"]; // 3128760
  name = doc["name"]; // "Barcelona"
  cod = doc["cod"]; // 200

  
}

void loop() {


    //Here is to be able to run updates by OTA (On air Updates)
  ArduinoOTA.handle();

  //any custom code or logic goes here

  /*if (!mqttclient.connected()) {
    reconnect();
    
  }
*/
  mqttclient.loop();

  long now = millis();
  if (now - lastMsg > 2000) {  //This is the 2 seconds delay to send message every 2 seconds.
    lastMsg = now;
    ++value;
    snprintf (msg, 50, "hello world #%ld", value);
    Serial.print("Publish message: ");
    Serial.println(msg);

    makeHTTPRequest();
    Serial.println("The content is:");
    Serial.println(content);



    //String json = http.getString();
    //Serial.println("recievedFile:");
    //Serial.print(json);
    /*
    deserializeJson(doc, content);
    float coord_lon = doc["coord"]["lon"]; // 2.16
    float coord_lat = doc["coord"]["lat"];

    float main = doc["weather"]["main"];

    Serial.print("Coord_lon:");
    Serial.println(coord_lon);
    Serial.print("Coord_lat:");
    Serial.println(coord_lat);
    Serial.print("Main time today:");
    Serial.println(main);x
    */
    refreshVariables();
   
    Serial.print("Main temperature:");
    Serial.println(main_temp);
    content = "";
    //client.publish("outTopic", msg);
    //client.publish("casa/despacho/temperatura", msg);


    // ----------------------------------------------------------------------------------------------------
    //delay(5000); //run every 5 seconds

    /*
    int linecounter=0;
    String vindchillString;
    float vindchill;
    String line="";
    ++value;

    Serial.print("connecting to ");
    Serial.println(host);

    // Use WiFiClient class to create TCP connections
   // WiFiClient client;
    const int httpPort = 80;
    if (!espClient.connect(host, httpPort)) {
        Serial.println("connection failed");
        return;
    }

    // We now create a URI for the request
    String url = "openweathermap.org/data/2.5/weather/?appid=439d4b804bc8187953eb36d2a8c26a02&id=3128760&units=metric";

    Serial.print("Requesting URL: ");
    Serial.println(url);

    // This will send the request to the server
    espClient.print(String("GET ") + url + " HTTP/1.1\r\n" +
                "Host: " + host + "\r\n" +
                "Connection: close\r\n\r\n");
    
    delay(10); //wait a while to let the server respond (increase on shitty connections)

    // Read all the lines of the reply from server
   
    Serial.println("httpRequest");
    httpRequest(url);
      */





        /*line = client.readStringUntil('\r'); //cariage return as delimiter
        //Serial.println(line);
        linecounter++;
        if(linecounter==31) vindchillString=line.substring(53,58);  //magic numbers galore :P*/



    
      /*
          if(line!="") //if data exists
          {
          vindchill=vindchillString.toFloat(); //parse the string as a float.
          Serial.println("String: " + String(vindchillString)); //test report
          Serial.println("float: " + String(vindchill)); //test report
          }

          Serial.println();
          Serial.println("closing connection");
      */
    //-------------------------------------------------------------------------------------------------------







    //getAndSendData();
    
  }
  
}