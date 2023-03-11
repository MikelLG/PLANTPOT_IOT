#include <Arduino.h>

#include <DHT.h>

// Constants
#define DHTPIN 5     // what pin we're connected to
#define DHTTYPE DHT22   // DHT 22  (AM2302)

String temp_str; //see last code block below use these to convert the float that you get back from DHT to a string =str
String hum_str;   
char temp[50];
char hum[50];

long interval = 0;

DHT dht(DHTPIN, DHTTYPE); //// Initialize DHT sensor for normal 16mhz Arduino

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.println("Booting..");
  dht.begin();

  }


void getAndSendTemperatureAndHumidityData(){
  Serial.println("Collecting temperature data.");

  // Reading temperature or humidity takes about 250 milliseconds!
  float h = dht.readHumidity();
  // Read temperature as Celsius (the default)
  float t = dht.readTemperature();
  

  // Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(t)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  Serial.print("Humidity: ");
  Serial.print(h);
  Serial.print(" %\t");
  Serial.print("Temperature: ");
  Serial.print(t);
  Serial.print(" *C ");

  String temperature = String(t);
  String humidity = String(h);

  /*
  // Just debug messages
  Serial.print( "Sending temperature and humidity : [" );
  Serial.print( temperature ); Serial.print( "," );
  Serial.print( humidity );
  Serial.print( "]   -> " );
  */
}

void loop() {

  long now = millis();
  if (now - interval > 2000) {  //This is the 2 seconds delay to send message every 2 seconds.
    interval = now;
    //++value;
    //snprintf (msg, 50, "hello world #%ld", value);
    //Serial.print("Publish message: ");
    //Serial.println(msg);

    //client.publish("outTopic", msg);
    //client.publish("casa/despacho/temperatura", msg);
    getAndSendTemperatureAndHumidityData();
  }

}