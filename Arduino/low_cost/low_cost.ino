
//this is an example of using the Adafruit ESP8266 Huzzah board as a low cost, displayless, uploader for the
//Sandsprite IoT humidor project. see readme.txt 

#include <ESP8266WiFi.h>
#include <DHT.h>
#include "private.h"

#define DHTTYPE DHT22
#define DHTPIN  2

String url = "/humidor/logData.php";
     
// Initialize DHT sensor 
// NOTE: For working with a faster than ATmega328p 16 MHz Arduino chip, like an ESP8266,
// you need to increase the threshold for cycle counts considered a 1 or 0.
// You can do this by passing a 3rd parameter for this threshold.  It's a bit
// of fiddling to find the right value, but in general the faster the CPU the
// higher the value.  The default for a 16mhz AVR is a value of 6.  For an
// Arduino Due that runs at 84mhz a value of 30 works.
// This is for the ESP8266 processor on ESP-01 
DHT dht(DHTPIN, DHTTYPE, 11); // 11 works fine for ESP8266
 
float humi, temp;  // Values read from sensor
int speedMode = 0;
uint8_t powerevt  = 1;
char tmp[100];

void setup() {
      Serial.begin(115200);
      delay(100);
     
      // We start by connecting to a WiFi network
      Serial.println();
      Serial.println();
      Serial.print("Connecting to ");
      Serial.println(ssid);
      
      WiFi.begin(ssid, password);
      
      while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
      }
     
      Serial.println("");
      Serial.println("WiFi connected");  
      Serial.println("IP address: ");
      Serial.println(WiFi.localIP());
}
    
void loop() {

      Serial.print("connecting to ");
      Serial.println(host);
      
      gettemperature();       // read sensor
      
      WiFiClient client;
      const int httpPort = 80;
      if (!client.connect(host, httpPort)) {
        Serial.println("connection failed");
        return;
      }

      Serial.print("Requesting URL: ");
      Serial.println(url);

      sprintf(tmp, "?temp=%d&humi=%d&powerevt=%d", (int)temp, (int)humi,  powerevt);
      url+=tmp;
      
      sprintf(tmp, "&clientid=%d&apikey=%s", client_id, APIKEY);
      url+=tmp;

      // This will send the request to the server
      client.print(String("GET ") + url + " HTTP/1.1\r\n" +
                   "Host: " + host + "\r\n" + 
                   "Connection: close\r\n\r\n");
      delay(500);
      
      // Read all the lines of the reply from server print first and last to Serial
      String line;
      bool isFirst = true;
      
      while(client.available()){
        line = client.readStringUntil('\r');
        if(isFirst){ Serial.print(line); isFirst = false;}
      }
      
      Serial.print(line); 
      
      Serial.println();
      Serial.println("closing connection");
      powerevt=0;
      
      delay_x_min(2);
}
    
void gettemperature() {
     
      // Reading temperature for humidity takes about 250 milliseconds!
      // Sensor readings may also be up to 2 seconds 'old' (it's a very slow sensor)
      humi = dht.readHumidity();          // Read humidity (percent)
      temp = dht.readTemperature(true);     // Read temperature as Fahrenheit
      
      if (isnan(humi) || isnan(temp)) {
          Serial.println("Failed to read from DHT sensor!");
          humi = 66; temp = 66; //probably not hooked up, just use test data...
      }

}

void delay_x_min(int minutes){
  
    Serial.print("delay for ");
    Serial.println(minutes);
 
    for(int i=0; i < minutes; i++){
        for(int j=0; j < 240; j++){ //entire j loop = one minute, using small delay so buttons responsive.. 
  	  delay( (speedMode==1 ? 1 : 250) );  
        }
        Serial.print(minutes-i);
        Serial.println(" until next upload...");
    }
  
}
