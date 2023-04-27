#include <Arduino.h>
/*
  BME280 // SR602 // TEMP6000 // SD CARD MANAGEMENT
  This is the main file for the I-pot prototype
  Developed by Mikel Llobera Guelbenzu. mikelguelbenzu@gmail.com https://github.com/MikelLG
  Mikel : "FILE WORKING ON Adafruit ESP32 Feather

  //PIR
  SR602 Pir Motion Sensor
  modified on 21 Oct 2020
  by Mohammad Reza Akbari @ Electropeak
*/

#include <Arduino.h>

// Libraries needed to use the microSD Card Adapter
#include "FS.h"
#include "SD.h"
#include "SPI.h"
#include "SD_CARD.h"
// Audio librar
#include "Audio.h"

// Libraries needed to run I2C for the BME280 sensor
#include <Wire.h>




// Digital I/O used
#define SD_CS          5

// Speakers Pin definitions
#define I2S_DOUT      33
#define I2S_BCLK      27
#define I2S_LRC       15


const int MAX_PLAYLIST_SIZE = 10;
String playlist[MAX_PLAYLIST_SIZE];
int playlist_size = 0;



// Declare the states. Enums start enumerating at zero, incrementing stemps of 1 unless overriden.
// We use an enum 'class' here for type safety and code readability.
enum class potState : uint8_t {
  IDLE,     // defaults to 0
  READING,  // defaults to 1
  PLAYING,  // defaults to 2 
  ALERT,    // defaults to 3
  ALARM,    // defualts to 4
};

// Keep track of the current State (it's a potState variable)
static potState currState = potState::IDLE;





// This is the state of the WAV Player
bool playing = false;
int counter = 0;

// pin for selecting the wav file
const int buttonPin = 13;

// array for storing file names
char *fileNames[] = {"estoyaqui.wav","Hola.wav", "Hola1.wav"};
const char *newFileNames[20] = {};
// index of the selected file
int selectedIndex = 0;
int numFiles = sizeof(fileNames) / sizeof(fileNames[0]);

int finished = 0;

File audioFile;

Audio audio; // Instance of the I2S player library

// Period and time_now for the millis function.
int period = 10 * 1000;    // Frio delay to not repeat S * ms.
unsigned long time_now = 0;

int period1 = 1 * 1000; 
unsigned long time_now1 = 0;

int period2 = 20 * 1000; 
unsigned long time_now2 = 0;



void add_to_playlist(String filename) {
  if (playlist_size < MAX_PLAYLIST_SIZE) {
    playlist[playlist_size] = filename;
    playlist_size++;
    Serial.print("Added ");
    Serial.println(filename);
  } else {
    Serial.println("Playlist is full");
  }
}

void print_playlist() {
  Serial.println("Playlist:");
  for (int i = 0; i < playlist_size; i++) {
    Serial.print(i);
    Serial.print(". ");
    Serial.println(playlist[i]);
  }
}

void remove_from_playlist(int index) {
  if (index >= 0 && index < playlist_size) {
    String filename = playlist[index];
    for (int i = index; i < playlist_size - 1; i++) {
      playlist[i] = playlist[i + 1];
    }
    playlist_size--;
    Serial.print("Removed ");
    Serial.println(filename);
  } else {
    Serial.println("Invalid index");
  }
}


// This function deletes the file not by the index, it finds the index with the name of the wav file.
void remove_from_playlist_name(String filename) {
  int index = -1;
  for (int i = 0; i < playlist_size; i++) {
    if (playlist[i] == filename) {
      index = i;
      break;
    }
  }
  if (index >= 0) {
    remove_from_playlist(index);
  } else {
    Serial.println("File not found");
  }
}


void deleteOldWav(){

  char fileName[20];
  snprintf(fileName, 20, "/%s", fileNames[selectedIndex]);
  if (selectedIndex > 1)
  {
    for(int i = 1; i < (selectedIndex - 1); i++) {
    strcpy(fileNames[i-1], fileNames[i]);
    }
  }
}

// Optional audio functions
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
    finished = 1;
       
}


// Function to run one wav when called.
void playWavFile(char* audioFile){
  
  finished = 0;
  audio.connecttoFS(SD, audioFile);
  playing = true;
  
}





void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);


  // configure the input pin for selecting the file
  pinMode(buttonPin, INPUT_PULLUP);





  audio.setTone(-30, 0,0);

  // This are the pins for the SD Card Reader 
  pinMode(SD_CS, OUTPUT);
  digitalWrite(SD_CS, HIGH);

  // Initialization of the SD Card
  SD.begin(SD_CS);

  // Set pinout and volume for the audio library
  audio.setPinout(I2S_BCLK, I2S_LRC, I2S_DOUT);
  audio.setVolume(18); // 0...21

  /*
  // Only testing volume
  int volume = audio.getVolume();
  volume--;
  audio.setVolume(volume);
  */


  if(!SD.begin(5)){
    Serial.println("Card Mount Failed");
    return;
  }

  // microSD card Initialization 
  uint8_t cardType = SD.cardType();

  if(cardType == CARD_NONE){
    Serial.println("No SD card attached");
    return;
  }

  Serial.print("SD Card Type: ");
  if(cardType == CARD_MMC){
    Serial.println("MMC");
  } else if(cardType == CARD_SD){
    Serial.println("SDSC");
  } else if(cardType == CARD_SDHC){
    Serial.println("SDHC");
  } else {
    Serial.println("UNKNOWN");
  }


  uint64_t cardSize = SD.cardSize() / (1024 * 1024);
  Serial.printf("SD Card Size: %lluMB\n", cardSize);





  add_to_playlist("estoyaqui.wav");
  add_to_playlist("Hola.wav");
  add_to_playlist("Hola1.wav");
  remove_from_playlist(1);


}


/*
void deleteOldWav(int selectedIndex){
  for(int i = 1; i < (selectedIndex - 1); i++) {
  strcpy(fileNames[i-1], fileNames[i]);
  }
}
*/




void loop() {
  // put your main code here, to run repeatedly:



  audio.loop();

  if(millis() >= time_now1 + period1){
    time_now1 += period1;

    if (digitalRead(buttonPin) == LOW) {
    selectedIndex = (selectedIndex + 1) % numFiles;
    Serial.print("Button Pressed");
    print_playlist();
    //delay(200);
    }

    char fileName[20];
    snprintf(fileName, 20, "/%s", fileNames[selectedIndex]);
    //audioFile = SD.open(fileName);

 
    
    if ((fileName) && (!playing)) {
    //audio.play(fileName);
    //deleteOldWav(selectedIndex);
    delay(1000);
    Serial.println(playing);
    playWavFile(fileName);
    Serial.print("Playing:");
    Serial.println(fileName);
    Serial.print("Index:");
    Serial.println(selectedIndex);
    Serial.println(playing);
    Serial.println(finished);
    }

    

  //delay(500);
  //audio.stop();
  //audioFile.close();

  }

  
  
    
  

  

// -----------------------
/*
  // open the selected file
  File file = SD.open(fileNames[selectedFile]);
*/
  // play the file if it exists

/*
  delay(500); // wait a bit before checking pin again

*/
  //audio.loop();

}

// 

/*

Igual es buena idea tener una lista donde poder poner y quitar cosas de la lista, dependiendo de los estados.
Tengo una funcion que siempre que haya algo en la lista toca algo a menos que ya este sonando algo.
No usar While ni delay.
Estado -- Planta esta hablando. -- Planta dejo de hablar. Estados que añaden o quitan audios a la lista.
En en loop solo leo sensores y modifico el estado con los sensores y toco lo que este en playlist. 
El playlist solo se modifica dependiendo de lo que los sensores me dicen.

Si el usuario siente que no le esta haciendo caso se molesta. Por eso es importante que siempre leas los sensores y actualizes las actuaciones en cada loop.

*/

