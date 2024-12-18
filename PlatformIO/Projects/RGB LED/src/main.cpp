#include <Arduino.h>
/*
 * Created by ArduinoGetStarted.com
 *
 * This example code is in the public domain
 *
 * Tutorial page: https://arduinogetstarted.com/tutorials/arduino-rgb-led
 */

const int PIN_RED   = 4;
const int PIN_GREEN = 16;
const int PIN_BLUE  = 17;

void setup() {
  pinMode(PIN_RED,   OUTPUT);
  pinMode(PIN_GREEN, OUTPUT);
  pinMode(PIN_BLUE,  OUTPUT);
}

void loop() {
  // color code #00C9CC (R = 0,   G = 201, B = 204)

  // Serial.println("color #00C9CC");
  // analogWrite(PIN_RED,   0);
  // analogWrite(PIN_GREEN, 201);
  // analogWrite(PIN_BLUE,  204);

  // delay(1000); // keep the color 1 second

  // // color code #F7788A (R = 247, G = 120, B = 138)
  // Serial.println("color #F7788A");
  // analogWrite(PIN_RED,   247);
  // analogWrite(PIN_GREEN, 120);
  // analogWrite(PIN_BLUE,  138);

  // delay(1000); // keep the color 1 second

  // color code #34A853 (R = 52,  G = 168, B = 83)
  Serial.println("color #34A853");
  analogWrite(PIN_RED,   0);
  analogWrite(PIN_GREEN, 204);
  analogWrite(PIN_BLUE,  255);

  delay(1000); // keep the color 1 second
}
