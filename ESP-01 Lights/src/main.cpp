#include <Arduino.h>

#include <WiFiManager.h>

#include <ESP8266WiFi.h>

#include "ESPAsyncTCP.h"

#include "aREST.h"
#include "aREST_UI.h"

aREST_UI rest = aREST_UI();
/*
const char* ssid = "LIGMA";
const char* password = "BALLS010";*/

int relayPin = 0;

long lastMsg = 0;

WiFiServer server(80);

String relayPinState = "off";

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);

  Serial.println("Booting...");

  rest.title("Relay Control");
  rest.button(0);

  rest.set_id("1");
  rest.set_name("esp8266");

  //pinMode(BUILTIN_LED, OUTPUT);     // Initialize the BUILTIN_LED pin as an output
  pinMode(relayPin, OUTPUT);

  digitalWrite(relayPin, LOW);

  /**/
  //-------------------
  WiFiManager wifiManager;

  wifiManager.autoConnect("APNAME");

  Serial.println("connected...yeey :)");

  WiFi.hostname("RELAY_CONTROL");

  Serial.println(WiFi.localIP());

  server.begin();
  Serial.println("Server started");



}

void loop() {
  // put your main code here, to run repeatedly:

  WiFiClient client = server.available();
  if (!client) {
    return;
  }
  while(!client.available()){
    delay(1);
  }

  rest.handle(client);


  long now = millis();
  if (now - lastMsg > 1000){
    lastMsg = now;
    //char msg = relayOn;
    //mqtt_client.publish("system/relay01/state", relay);
  }
}