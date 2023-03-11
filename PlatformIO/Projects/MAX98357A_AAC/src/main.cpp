// This code is to play an AAC audio from an external .h and reproduce it with MAX98357A and a speaker.
// Mikel : Working!



#include <Arduino.h>
#include "AudioGeneratorAAC.h"
#include "AudioOutputI2S.h"
#include "AudioFileSourcePROGMEM.h"
#include "sampleaac.h"
//#include "Quepasa.h"

//#include <Adafruit_MAX98357.h>

const int I2S_BCLK = 27;
const int I2S_LRC = 15;
const int I2S_DOUT = 33;

AudioFileSourcePROGMEM *in;
AudioGeneratorAAC *aac;
AudioOutputI2S *out;

void setup()
{
  Serial.begin(115200);

  audioLogger = &Serial;
  in = new AudioFileSourcePROGMEM(sampleaac, sizeof(sampleaac));
  aac = new AudioGeneratorAAC();
  //out = new AudioOutputI2S(I2S_BCLK, I2S_LRC, I2S_DOUT);
  out = new AudioOutputI2S();
  out -> SetGain(1);// Adjust the volume mikel
  out -> SetOutputModeMono(true);
  out -> SetPinout(I2S_BCLK, I2S_LRC, I2S_DOUT);

  aac->begin(in, out);
}


void loop()
{
  if (aac->isRunning()) {
    aac->loop();
  } else {
    aac -> stop(); //mikel
    Serial.printf("AAC done\n");
    delay(1000);
  }
}

