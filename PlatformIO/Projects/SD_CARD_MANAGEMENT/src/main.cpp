/*

  Mikel : "FILE WORKING ON ESP32
  Rui Santos
  Complete project details at https://RandomNerdTutorials.com/esp32-microsd-card-arduino/
  
  This sketch can be found at: Examples > SD(esp32) > SD_Test
*/

/*  SPI PINS
SD CARD READER / ESP32
MISO - 19
MOSI - 23
SCK - 18
CS - 5
*/
  
#include "Arduino.h"
#include "FS.h"
#include "SD.h"
#include "SPI.h"
#include "SD_CARD.h"


void setup(){
  Serial.begin(115200);
  if(!SD.begin(5)){
    Serial.println("Card Mount Failed");
    return;
  }
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

  listDir(SD, "/", 0);                       // This lists the directories into the sd card
  createDir(SD, "/mydir");                   // This creates a directory called mydir
  listDir(SD, "/", 0);                       // This lists the directories into the sd card again
  removeDir(SD, "/mydir");                   // This removes the directory mydir
  listDir(SD, "/", 2);                       // This lists the directories again.
  writeFile(SD, "/hello.txt", "Hello ");     // This writes and creates a file called hello.txt and writes "Hello " on it.
  appendFile(SD, "/hello.txt", "World!\n");  // This appends "World" into the hello.txt file.
  readFile(SD, "/hello.txt");                // This reads the file hello.txt
  deleteFile(SD, "/foo.txt");                // This delete a file called foo.txt
  renameFile(SD, "/hello.txt", "/foo.txt");  // This renames the file hello.txt into foo.txt
  readFile(SD, "/foo.txt");                  // This reads the file foo.txt
  testFileIO(SD, "/test.txt");               // Test the file test.txt
  Serial.printf("Total space: %lluMB\n", SD.totalBytes() / (1024 * 1024));  // This shows in serial the total space available into the SD Card.
  Serial.printf("Used space: %lluMB\n", SD.usedBytes() / (1024 * 1024));  // This shows in serial the total space occupied into the SD Card.
}

void loop(){

}