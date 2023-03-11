#include <Arduino.h>

    /*
  SR602 Pir Motion Sensor
  modified on 21 Oct 2020
  by Mohammad Reza Akbari @ Electropeak
  Home
*/

int ledPin = 13;                // LED
int pirPin = 15;                 // PIR Out pin
int pirStat = 0;                // PIR status
void setup() {
  pinMode(ledPin, OUTPUT);
  pinMode(pirPin, INPUT);
  Serial.begin(115200);
}
void loop() {
  pirStat = digitalRead(pirPin);
  if (pirStat == HIGH) {
    digitalWrite(ledPin, HIGH);
    Serial.println("Hey I got you!!!");
  }
  else {
    digitalWrite(ledPin, LOW);
  }
}