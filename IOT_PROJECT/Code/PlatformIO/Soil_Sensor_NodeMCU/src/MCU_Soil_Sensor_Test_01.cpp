#include <Arduino.h>
#include <ESP8266WiFi.h>          //https://github.com/esp8266/Arduino
#include <WiFiManager.h>  
#include <PubSubClient.h> 

int sensor_pin = A0;

const char* mqtt_server = "192.168.1.188";

int output_value ;

String hum_str; //see last code block below use these to convert the float that you get back from DHT to a string =str
char hum[50];


WiFiClient espClientSoilSensor;
PubSubClient client(espClientSoilSensor);
long lastMsg = 0;
char msg[50];
long int value = 0;

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
      client.publish("room/soilsensor/humidity", "Enviando el primer mensaje");
      // ... and resubscribe
      //client.subscribe("inTopic");
      //client.subscribe("casa/despacho/luz");
      
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

Serial.begin(9600);
Serial.println("Booting");

client.setServer(mqtt_server, 1883);
//client.setCallback(callback);

WiFiManager wifiManager;

wifiManager.autoConnect("APNAME");
Serial.println("connected...yeey :)");

Serial.println("Reading From the Sensor ...");

delay(2000);

}

void getAndSendSensorData(){

output_value= analogRead(sensor_pin);
output_value = map(output_value,770,300,0,100);

Serial.print("Mositure : ");

Serial.print(output_value);

Serial.println("%");
//soilHum = output_value;
hum_str = String(output_value);
hum_str.toCharArray(hum, hum_str.length() + 1);

client.publish("room/soilsensor/humidity", hum); 

delay(1000);

}
void loop() {

if (!client.connected()) {
    reconnect();
  }
  client.loop();
//getAndSendSensorData();
long now = millis();
    if (now - lastMsg > 2000) {
    lastMsg = now;
    ++value;
    snprintf (msg, 50, "hello world #%ld", value);
    Serial.print("Publish message: ");
    Serial.println(msg);
    //client.publish("outTopic", msg);
    //client.publish("casa/despacho/temperatura", msg);
    getAndSendSensorData();
    }
}
