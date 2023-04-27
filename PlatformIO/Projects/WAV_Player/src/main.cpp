#include "Arduino.h"
#include "Audio.h"
#include "SPI.h"
#include <SerialCommands.h>

// Digital I/O used
#define SD_CS          5
//#define SPI_MOSI      23
//#define SPI_MISO      19
//#define SPI_SCK       18
#define I2S_DOUT      33
#define I2S_BCLK      27
#define I2S_LRC       15

bool playing = false;
int counter = 0;

Audio audio;

int period = 4000;
unsigned long time_now = 0;


void setup() {
    pinMode(SD_CS, OUTPUT);
    digitalWrite(SD_CS, HIGH);

    //SPI.begin(SPI_SCK, SPI_MISO, SPI_MOSI);

    //SPI.setFrequency(1000000);

    Serial.begin(115200);
    SD.begin(SD_CS);

    audio.setPinout(I2S_BCLK, I2S_LRC, I2S_DOUT);
    audio.setVolume(15); // 0...21

    //audio.connecttoFS(SD, "/wavfile.wav");
    
}

void playWavFile(const char* file){
  
  audio.connecttoFS(SD, file);
  playing = true;
}

void loop()
{
  
  if(millis() >= time_now + period){
    time_now += period;
    if (counter == 0){
      playWavFile("wavfile.wav");
      counter++;
    }
    else if (counter == 1){
      playWavFile("wavfile1.wav");
      counter++;
    }
    else if (counter == 2){
      playWavFile("wavfile2.wav");
      counter++;
    }
    else{

    }
    Serial.println("Next sound launched");
  }

  while (playing == true){
    audio.loop();
  }

}

// optional
void audio_info(const char *info){
    Serial.print("info        "); Serial.println(info);
}
void audio_id3data(const char *info){  //id3 metadata
    Serial.print("id3data     ");Serial.println(info);
}
void audio_eof_mp3(const char *info){  //end of file
    Serial.print("eof_mp3     ");Serial.println(info);
    playing = false;
    Serial.println("Finished playing");
}