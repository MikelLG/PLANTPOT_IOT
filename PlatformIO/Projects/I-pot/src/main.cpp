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
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>

//#define SEALEVELPRESSURE_HPA (1013.25)
#define SEALEVELPRESSURE_HPA (1020)
//#define BME_CS 21

// Define the pin for the TEMT6000 Sensor
#define LIGHTSENSORPIN 12 //Ambient light sensor pin

// Digital I/O used
#define SD_CS          5
//#define SPI_MOSI      23
//#define SPI_MISO      19
//#define SPI_SCK       18

// Speakers Pin definitions
#define I2S_DOUT      33
#define I2S_BCLK      27
#define I2S_LRC       15

// RGB LED Pin Definitions
#define PIN_RED       4
#define PIN_GREEN     16
#define PIN_BLUE      17

// Definition of the colors for the RGB LED
#define LED_WHITE  255, 255, 255
#define	LED_RED		 255, 0, 0
#define LED_BLUE   0, 0, 255
#define LED_GREEN  0, 255, 0
#define LED_PURPLE 255, 0, 255
#define LED_YELLOW 255, 255, 0
#define LED_CYAN   0, 255, 255
#define LED_ORANGE 255, 153, 51

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

// Instance of the BME Temp and Hum sensor.
Adafruit_BME280 bme;

// Pin definitions for SR602 Pir Sensor 
int ledPin = 13;                // LED
int pirPin = 14;                 // PIR Out pin
int pirStat = 0;                // PIR status
bool findsomeone = false;
bool found = false;

// This is the state of the WAV Player
bool playing = false;
int counter = 0;

float minAirTemp = 28;


// State of every sensor
float light;
float airTemp;
float pressure;
float altitude;
float airHum;
float soilTemp;
float soilHum;

bool frio = false;


Audio audio; // Instance of the I2S player library

// Period and time_now for the millis function.
int period = 10 * 1000;    // Frio delay to not repeat S * ms.
unsigned long time_now = 0;

int period1 = 1 * 1000; 
unsigned long time_now1 = 0;

int period2 = 20 * 1000; 
unsigned long time_now2 = 0;

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
}
void ledSetColor(int R, int G, int B) {
  analogWrite(PIN_RED,   R);
  analogWrite(PIN_GREEN, G);
  analogWrite(PIN_BLUE,  B);
}


// Function to run one wav when called.
void playWavFile(const char* file){
  
  audio.connecttoFS(SD, file);
  playing = true;
  
}

void foundSomeone(){

  if (!found){
      found = true;
      int rand = random(15);
      if (rand <= 4){
        Serial.print("Random:");
        Serial.println(rand);
        playWavFile("estoyaqui.wav");
      }
      else if (rand <= 10){
        playWavFile("Hola.wav");
      }
      else {
        playWavFile("Hola1.wav");
      }
    }
    else{
      // This piece of code limits the amount of times that it says that detected someone.
      if(millis() >= time_now2 + period2){
        time_now2 += period2;
        found = false;
      }
    }

}

// How to create a playlist of wav files depending on the sensor data º

// Reads the PIR Sensor 
void readPirSensor(){
  //VICTOR
  if (playing){
    return;
  }


  // Shows "Hey I got you!!" when the SR602 Pir Sensor detects movement
  pirStat = digitalRead(pirPin);
  if (pirStat == HIGH) {
    //digitalWrite(ledPin, HIGH);
    Serial.println("Hey I got you!!!");
    
    foundSomeone();
    
    Serial.println();
  }
  else {
    //digitalWrite(ledPin, LOW);
  }
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);

  audio.setTone(-30, 0,0);

  //Definition RGB LED Pins
  pinMode(PIN_RED, OUTPUT);
  pinMode(PIN_GREEN, OUTPUT);
  pinMode(PIN_BLUE, OUTPUT);

  ledSetColor(LED_YELLOW);



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

  // Definition of the pins for the SR602 Pir Sensor
  pinMode(ledPin, OUTPUT);
  pinMode(pirPin, INPUT);

  // Definition of the pin for the TEMT6000 Sensor
  pinMode(LIGHTSENSORPIN,  INPUT);

  // Initialization of the BME280 Sensor.
	if (!bme.begin(0x76)) {
		Serial.println("Could not find a valid BME280 sensor, check wiring!");
		while (1);
	}

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

  // listDir(SD, "/", 0);                       // This lists the directories into the sd card
  // createDir(SD, "/mydir");                   // This creates a directory called mydir
  // listDir(SD, "/", 0);                       // This lists the directories into the sd card again
  // removeDir(SD, "/mydir");                   // This removes the directory mydir
  // listDir(SD, "/", 2);                       // This lists the directories again.
  // writeFile(SD, "/hello.txt", "Hello ");     // This writes and creates a file called hello.txt and writes "Hello " on it.
  // appendFile(SD, "/hello.txt", "World!\n");  // This appends "World" into the hello.txt file.
  // readFile(SD, "/hello.txt");                // This reads the file hello.txt
  // deleteFile(SD, "/foo.txt");                // This delete a file called foo.txt
  // renameFile(SD, "/hello.txt", "/foo.txt");  // This renames the file hello.txt into foo.txt
  // readFile(SD, "/foo.txt");                  // This reads the file foo.txt
  // testFileIO(SD, "/test.txt");               // Test the file test.txt
  // Serial.printf("Total space: %lluMB\n", SD.totalBytes() / (1024 * 1024));  // This shows in serial the total space available into the SD Card.
  // Serial.printf("Used space: %lluMB\n", SD.usedBytes() / (1024 * 1024));  // This shows in serial the total space occupied into the SD Card.
  // //playWavFile("wavfile.wav");
  
}


void readSensors(){
  // Reads the Light Sensor
  light = analogRead(LIGHTSENSORPIN);   //Read light level
  float square_ratio = light / 4096;          //Get percent of maximum value (1023)
  square_ratio = pow(square_ratio, 2.0);        //Square to make response more obvious
  light = map(light,0,4095,0,100);          //Converts the reading into a percentage.
  analogWrite(ledPin, 255.0 * square_ratio);    //Adjust LED brightness relatively

  // Reads the BME280 Temp & Hum ambient Sensor
  airTemp = bme.readTemperature();
  pressure = bme.readPressure() / 100.0F;
  altitude = bme.readAltitude(SEALEVELPRESSURE_HPA);
  airHum = bme.readHumidity();


  if ((airTemp <= minAirTemp) && (playing == false)){
    if (!frio){
      playWavFile("TengoFrio1.wav");
      frio = true;
    }
    else {
      if(millis() >= time_now + period){
        time_now += period;
        frio = false;
      }
    }
    
  } 

}

void serialPrintSensors(){

  // This prints on serial the data gathered from the sensors.

  Serial.print("Light: ");
  Serial.print(light);                      //Display reading in serial monitor
  Serial.println("%");
  Serial.println();

  Serial.print("Temperature = ");
  Serial.print(airTemp);
  Serial.println("*C");

  Serial.print("Pressure = ");
  Serial.print(pressure);
  Serial.println("hPa");

  Serial.print("Approx. Altitude = ");
  Serial.print(altitude);
  Serial.println("m");

  Serial.print("Humidity = ");
  Serial.print(airHum);
  Serial.println("%");

  Serial.println();

}

void loop() {
  // put your main code here, to run repeatedly:

  if(millis() >= time_now1 + period1){
    time_now1 += period1;

    readSensors();
    serialPrintSensors();
    readPirSensor();
    
    Serial.println(time_now);
    Serial.println(time_now2);
    
  }
  audio.loop();
 
   // Quitar el while de aqui, para no interrumpir. Usar la variable de playing para tirar el siguiente audio o no.
   // If readPIRSensor true playwav. (cambiar funcion a if someone is around, then play)
   // Nunca te metas en un loop que te interrumpa.

  /*
  while (playing == true){
    audio.loop();
  }
  */
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

