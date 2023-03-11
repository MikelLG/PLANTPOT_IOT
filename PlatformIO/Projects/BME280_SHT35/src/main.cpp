#include <Arduino.h>


#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>

//#define SEALEVELPRESSURE_HPA (1013.25)
#define SEALEVELPRESSURE_HPA (1020)
//#define BME_CS 21

Adafruit_BME280 bme;

//          SHT 35 CODE
#include "Seeed_SHT35.h"


/*SAMD core*/
#ifdef ARDUINO_SAMD_VARIANT_COMPLIANCE
    #define SDAPIN  20
    #define SCLPIN  21
    #define RSTPIN  7
    #define SERIAL SerialUSB
#else
    #define SDAPIN  21
    #define SCLPIN  22
    #define RSTPIN  2   
   // #define SERIAL Serial
#endif

SHT35 sensor(SCLPIN);


void setup() {
	Serial.begin(115200);

  Serial.println("serial start!!");
  if (sensor.init()) {
      Serial.println("sensor init failed!!");
  }
  delay(1000);


	if (!bme.begin(0x76)) {
		Serial.println("Could not find a valid BME280 sensor, check wiring!");
		while (1);
	}
}

void loop() {

  u16 value = 0;
  u8 data[6] = {0};
  float temp, hum;
  if (NO_ERROR != sensor.read_meas_data_single_shot(HIGH_REP_WITH_STRCH, &temp, &hum)) {
      Serial.println("read temp failed!!");
      Serial.println("   ");
      Serial.println("   ");
      Serial.println("   ");
  } else {

      Serial.print(bme.readTemperature());
      Serial.print(",");
      Serial.print(temp);
      Serial.print(",");
      Serial.print(bme.readHumidity());
      Serial.print(",");
      Serial.println(hum);
  }

  delay(1000);




	////////////
  /*
	Serial.print("Temperature = ");
	Serial.print(bme.readTemperature());
	Serial.println("*C");

	Serial.print("Pressure = ");
	Serial.print(bme.readPressure() / 100.0F);
	Serial.println("hPa");

	Serial.print("Approx. Altitude = ");
	Serial.print(bme.readAltitude(SEALEVELPRESSURE_HPA));
	Serial.println("m");

	Serial.print("Humidity = ");
	Serial.print(bme.readHumidity());
	Serial.println("%");

	Serial.println();
	delay(1000);
  */
}