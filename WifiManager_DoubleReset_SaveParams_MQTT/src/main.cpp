//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  
//    This code is an example of how to use an ESP32 to connect to a Wifi without hard coding the WiFi credentials.
//    If the ESP32 doesn't have a connection saved into its file and can't connect to internet, it will start an AP Access Point.
//    Once the ESP32 has connection to a network it will then try to connect to the MQTT Broser to send the data of the sensors.
//    If you press twice the ResetButton of the ESP32, it will reset the Wifi Connection and launch again the AP Access Point to input the credentials.
//
//    Modified my Mikel LLobera Guelbenzu on 16/11/22. (mikelguelbenzu@gmail.com)
//
//    Works with ESP32
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


#include <Arduino.h>

#include <FS.h>                   //this needs to be first, or it all crashes and burns...

// Libraries 
#include <DHT.h>

#include <DNSServer.h>

// SPI Flash Syetem Library
#include <SPIFFS.h>

// Librerias necesarias para Hacer del ESP un WebServer.
#include <WiFi.h>
#include <WebServer.h>

// Library for using the WifiManager -- This allows you to not hardcode the Wifi Credentials!
//#include <ESP_WifiManager.h>
#include <WiFiManager.h>

#include <ArduinoJson.h>          //https://github.com/bblanchon/ArduinoJson

/////////////////////////------------------------------------------DOUBLE RESET DETECTION vv---------------------------
// These defines must be put before #include <ESP_DoubleResetDetector.h>
// to select where to store DoubleResetDetector's variable.
// For ESP32, You must select one to be true (EEPROM or SPIFFS)
// For ESP8266, You must select one to be true (RTC, EEPROM or SPIFFS)
// Otherwise, library will use default EEPROM storage
#define ESP_DRD_USE_EEPROM      false
#define ESP_DRD_USE_SPIFFS      true    //false

#ifdef ESP8266
  #define ESP8266_DRD_USE_RTC     false   //true
#endif

#define DOUBLERESETDETECTOR_DEBUG       true  //false

#include <ESP_DoubleResetDetector.h>      //https://github.com/khoih-prog/ESP_DoubleResetDetector

// Number of seconds after reset during which a
// subseqent reset will be considered a double reset.
#define DRD_TIMEOUT 10

// RTC Memory Address for the DoubleResetDetector to use
#define DRD_ADDRESS 0

DoubleResetDetector* drd;


/////////////////////////------------------------------------------DOUBLE RESET DETECTION ^^---------------------------



#include <PubSubClient.h>   //MQTT COMUNICATION


#define LED_BUILTIN 13


//define your default values here, if there are different values in config.json, they are overwritten.
/*char ssid_id[40] = "";
char ssid_psw[40] = "";*/
char static_ip[40] = "192.168.1.200";
char static_gateway[40] = "192.168.1.1";
char subnet[40] = "255.255.255.0";

char mqtt_server[40] = "192.168.1.42";
char mqtt_port[6] = "1883";
char blynk_token[34] = "YOUR_BLYNK_TOKEN";

//flag for saving data
bool shouldSaveConfig = false;

//callback notifying us of the need to save config
void saveConfigCallback () {
  Serial.println("Should save config");
  shouldSaveConfig = true;
}
// Variables to hold data from custom textboxes
char testString[50] = "test value";
int testNumber = 1234;


String temp_str; //see last code block below use these to convert the float that you get back from DHT to a string =str
String hum_str;   
char temp[50];
char hum[50];


// Constants
#define DHTPIN 5     // what pin we're connected to
#define DHTTYPE DHT22   // DHT 22  (AM2302)

// MQTT COMMNICATION
WiFiClient espClient;     //What exactly this does? *****************
PubSubClient client(espClient);
long lastMsg = 0;
char msg[50];
int value = 0;

DHT dht(DHTPIN, DHTTYPE); //// Initialize DHT sensor for normal 16mhz Arduino

//Variables
int chk;
//float hum;  //Stores humidity value
//float temp; //Stores temperature value

void callback(char* topic, byte* payload, unsigned int length) {      /// ME HE QUEDADO AQUI!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  String topicStr = topic;
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]); 
  }
  Serial.println();

  // Switch on the LED if an 1 was received as first character
  if ((char)payload[0] == '1') {
    digitalWrite(LED_BUILTIN, LOW);   // Turn the LED on (Note that LOW is the voltage level
    // but actually the LED is on; this is because
    // it is active low on the ESP-01)
    //Serial.println("The payload is ", (String)payload);
    //Serial.println("Led should be turned ON");
  } else {
    digitalWrite(LED_BUILTIN, HIGH);  // Turn the LED off by making the voltage HIGH
  }

  if(topicStr =="system/light")
  {
    if (payload[0] == '1'){
      digitalWrite(LED_BUILTIN, LOW);
      Serial.println("Led should be turned ON");
    }
    else if (payload[0] == '0'){
      digitalWrite(LED_BUILTIN, HIGH);
      Serial.println("Led should be turned OFF");
    }
  }

}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP8266Client-SoilSensor";
    //clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str())) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      //client.publish("outTopic", "hello world");
      client.publish("dht22/temperature", "Enviando el primer mensaje");
      // ... and resubscribe
      //client.subscribe("inTopic");
      client.subscribe("system/light");
      
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void readFS(){
   //read configuration from FS json
  Serial.println("mounting FS...");

  if (SPIFFS.begin()) {
    Serial.println("mounted file system");
    if (SPIFFS.exists("/config.json")) {
      //file exists, reading and loading
      Serial.println("reading config file");
      File configFile = SPIFFS.open("/config.json", "r");
      if (configFile) {
        Serial.println("opened config file");
        size_t size = configFile.size();
        // Allocate a buffer to store contents of the file.
        std::unique_ptr<char[]> buf(new char[size]);

        configFile.readBytes(buf.get(), size);
        DynamicJsonDocument jsonBuffer(1024);
        DeserializationError error = deserializeJson(jsonBuffer,buf.get());
        //JsonObject& json = jsonBuffer.parseObject(buf.get());
        //jsonBuffer.printTo(Serial);

        if (!error) {
          Serial.println("\nparsed json");
          
          /*strcpy(ssid_id, jsonBuffer["ssid_id"]);
          strcpy(ssid_psw, jsonBuffer["ssid_psw"]);*/
          strcpy(static_ip, jsonBuffer["static_ip"]);
          strcpy(static_gateway, jsonBuffer["static_gateway"]);
          strcpy(subnet, jsonBuffer["subnet"]);


          strcpy(mqtt_server, jsonBuffer["mqtt_server"]);
          strcpy(mqtt_port, jsonBuffer["mqtt_port"]);
          strcpy(blynk_token, jsonBuffer["blynk_token"]);

        } else {
          Serial.println("failed to load json config");
        }
        configFile.close();
      }
    }
  }
  else {
    Serial.println("failed to mount FS");
  }

  //End read!

}

void saveFS(){

  //save the custom parameters to FS
  if (shouldSaveConfig) {
    Serial.println("saving config");
    DynamicJsonDocument jsonBuffer(1024);
    //JsonObject& json = jsonBuffer.createObject();
    //jsonBuffer[""]


    //jsonBuffer["ssid_id"] = ssid_id;
    //jsonBuffer["ssid_psw"] = ssid_psw;
    jsonBuffer["static_ip"] = static_ip;
    jsonBuffer["static_gateway"] = static_gateway;
    jsonBuffer["subnet"] = subnet;

    jsonBuffer["mqtt_server"] = mqtt_server;
    jsonBuffer["mqtt_port"] = mqtt_port;
    jsonBuffer["blynk_token"] = blynk_token;

    File configFile = SPIFFS.open("/config.json", "w");
    if (!configFile) {
      Serial.println("failed to open config file for writing");
    }

    serializeJson(jsonBuffer, Serial);
    serializeJson(jsonBuffer, configFile);
    //json.printTo(Serial);
    //json.printTo(configFile);
    
    configFile.close();
    //end save
  }

}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.println("Booting..");

  pinMode(LED_BUILTIN,OUTPUT);
  
  drd = new DoubleResetDetector(DRD_TIMEOUT, DRD_ADDRESS);

  
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);

  dht.begin();

  
  Serial.println();

  //clean FS, for testing
  //SPIFFS.format();
  readFS();
  
  



  // The extra parameters to be configured (can be either global or just in the setup)
  // After connecting, parameter.getValue() will get you the configured value
  // id/name placeholder/prompt default length
  /*
  WiFiManagerParameter custom_ssid_id("ssid", "ssid id", ssid_id, 40);
  WiFiManagerParameter custom_ssid_psw("psw", "ssid psw", ssid_psw, 40);*/
  WiFiManagerParameter custom_static_ip("ip", "static ip", static_ip, 40);
  WiFiManagerParameter custom_static_gateway("gate", "static gateway", static_gateway, 40);
  WiFiManagerParameter custom_subnet("subnet", "subnet ip", subnet, 40);
  
  WiFiManagerParameter custom_mqtt_server("server", "mqtt server", mqtt_server, 40);
  WiFiManagerParameter custom_mqtt_port("port", "mqtt port", mqtt_port, 6);
  WiFiManagerParameter custom_blynk_token("blynk", "blynk token", blynk_token, 32);

  //WiFiManager
  //Local intialization. Once its business is done, there is no need to keep it around
  WiFiManager wifiManager;

  //set config save notify callback
  wifiManager.setSaveConfigCallback(saveConfigCallback);

  //set static ip
  wifiManager.setSTAStaticIPConfig(IPAddress(192,168,1,200), IPAddress(192,168,1,1), IPAddress(255,255,255,0));
  

  
  //add all your parameters here
/*
  wifiManager.addParameter(&custom_ssid_id);
  wifiManager.addParameter(&custom_ssid_psw);*/
  wifiManager.addParameter(&custom_static_ip);
  wifiManager.addParameter(&custom_static_gateway);
  wifiManager.addParameter(&custom_subnet);
  
  wifiManager.addParameter(&custom_mqtt_server);
  wifiManager.addParameter(&custom_mqtt_port);
  wifiManager.addParameter(&custom_blynk_token);

  //reset settings - for testing
  //wifiManager.resetSettings();

  //set minimu quality of signal so it ignores AP's under that quality
  //defaults to 8%
  //wifiManager.setMinimumSignalQuality();
  
  //sets timeout until configuration portal gets turned off
  //useful to make it all retry or go to sleep
  //in seconds
  //wifiManager.setTimeout(120);

  //fetches ssid and pass and tries to connect
  //if it does not connect it starts an access point with the specified name
  //here  "AutoConnectAP"
  //and goes into a blocking loop awaiting configuration
  if (!wifiManager.autoConnect("AutoConnectAP", "password")) {
    Serial.println("failed to connect and hit timeout");
    delay(3000);
    //reset and try again, or maybe put it to deep sleep
    ESP.restart();
    delay(5000);
  }

  //if you get here you have connected to the WiFi
  
  Serial.println("connected...yeey :)");

  if (drd->detectDoubleReset())
  {
    Serial.println("Double Reset Detected");
    digitalWrite(LED_BUILTIN, LOW);
    wifiManager.resetSettings();
  }
  else
  {
    Serial.println("No Double Reset Detected");
    digitalWrite(LED_BUILTIN, HIGH);
  }
    

  //read updated parameters

/*
  strcpy(ssid_id, custom_ssid_id.getValue());
  strcpy(ssid_psw, custom_ssid_psw.getValue());*/
  strcpy(static_ip, custom_static_ip.getValue());
  strcpy(static_gateway, custom_static_gateway.getValue());
  strcpy(subnet, custom_subnet.getValue());

  
  strcpy(mqtt_server, custom_mqtt_server.getValue());
  strcpy(mqtt_port, custom_mqtt_port.getValue());
  strcpy(blynk_token, custom_blynk_token.getValue());
/*
  //save the custom parameters to FS
  if (shouldSaveConfig) {
    Serial.println("saving config");
    DynamicJsonDocument jsonBuffer(1024);
    //JsonObject& json = jsonBuffer.createObject();
    //jsonBuffer[""]


    //jsonBuffer["ssid_id"] = ssid_id;
    //jsonBuffer["ssid_psw"] = ssid_psw;
    jsonBuffer["static_ip"] = static_ip;
    jsonBuffer["static_gateway"] = static_gateway;
    jsonBuffer["subnet"] = subnet;

    jsonBuffer["mqtt_server"] = mqtt_server;
    jsonBuffer["mqtt_port"] = mqtt_port;
    jsonBuffer["blynk_token"] = blynk_token;

    File configFile = SPIFFS.open("/config.json", "w");
    if (!configFile) {
      Serial.println("failed to open config file for writing");
    }

    serializeJson(jsonBuffer, Serial);
    serializeJson(jsonBuffer, configFile);
    //json.printTo(Serial);
    //json.printTo(configFile);
    
    configFile.close();
    //end save
  }
*/
saveFS();

Serial.println("local ip");
Serial.println(WiFi.localIP());


   


}


void getAndSendTemperatureAndHumidityData(){
  Serial.println("Collecting temperature data.");

  // Reading temperature or humidity takes about 250 milliseconds!
  float h = dht.readHumidity();
  // Read temperature as Celsius (the default)
  float t = dht.readTemperature();
  

  // Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(t)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  Serial.print("Humidity: ");
  Serial.print(h);
  Serial.print(" %\t");
  Serial.print("Temperature: ");
  Serial.print(t);
  Serial.print(" *C ");

  String temperature = String(t);
  String humidity = String(h);


  // Just debug messages
  Serial.print( "Sending temperature and humidity : [" );
  Serial.print( temperature ); Serial.print( "," );
  Serial.print( humidity );
  Serial.print( "]   -> " );

  // Prepare a JSON payload string
  String payload = "{";
  payload += "\"temperature\":"; payload += temperature; payload += ",";
  payload += "\"humidity\":"; payload += humidity;
  payload += "}";

  // Send payload of the JSON
  char attributes[100];
  payload.toCharArray( attributes, 100 );
  client.publish( "dht22/state", attributes );
  Serial.println( attributes );



  temp_str = String(t); //converting temp (the float variable above) to a string 
  temp_str.toCharArray(temp, temp_str.length() + 1); //packaging up the data to publish to mqtt whoa...

  hum_str = String(h); //converting Humidity (the float variable above) to a string
  hum_str.toCharArray(hum, hum_str.length() + 1); //packaging up the data to publish to mqtt whoa...


  
    client.publish("dht22/temperature", temp); //money shot
    client.publish("dht22/humidity", hum);
}


void loop() {
  // put your main code here, to run repeatedly:
  

  // Call the double reset detector loop method every so often,
  // so that it can recognise when the timeout expires.
  // You can also call drd.stop() when you wish to no longer
  // consider the next reset as a double reset.
  drd->loop();

  if (!client.connected()) {
    reconnect();
  }
  client.loop();


  long now = millis();
  if (now - lastMsg > 2000) {  //This is the 2 seconds delay to send message every 2 seconds.
    lastMsg = now;
    //++value;
    //snprintf (msg, 50, "hello world #%ld", value);
    //Serial.print("Publish message: ");
    //Serial.println(msg);

    //client.publish("outTopic", msg);
    //client.publish("casa/despacho/temperatura", msg);
    getAndSendTemperatureAndHumidityData();
  }

}

