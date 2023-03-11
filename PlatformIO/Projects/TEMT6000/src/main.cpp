#include <Arduino.h>

//  Example to capture data from the ambient light with TEMP6000 sensor.
//  MIKEL: Working

#define LEDPIN 13         //LED brightness (PWM) writing
#define LIGHTSENSORPIN 12 //Ambient light sensor reading 

void setup() {
  pinMode(LIGHTSENSORPIN,  INPUT);  
  pinMode(LEDPIN, OUTPUT);  
  Serial.begin(115200);
}

void loop() {
  float reading = analogRead(LIGHTSENSORPIN); //Read light level
  float square_ratio = reading / 4096;      //Get percent of maximum value (1023)
  square_ratio = pow(square_ratio, 2.0);      //Square to make response more obvious
  reading = map(reading,0,4095,0,100);
  analogWrite(LEDPIN, 255.0 * square_ratio);  //Adjust LED brightness relatively
  Serial.println(reading);   //Display reading in serial monitor
  delay(200);                 
}