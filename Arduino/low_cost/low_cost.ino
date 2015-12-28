
//this is an example of using the Adafruit ESP8266 Huzzah board as a low cost, displayless, uploader for the
//Sandsprite IoT humidor project. see readme.txt 
//
//this has been tested and will gracefully handle the wifi access point going up and down.
//it also handles timeouts gracefully..the ESP8266 is a very nice chip!
//
//this should be pretty solid and ready for use..

#include <ESP8266WiFi.h>
#include <DHT.h>
#include "private.h"

#define DHTTYPE DHT22
#define DHTPIN  2

int speedMode = 0;
char* base_url = "/humidor/logData.php";
uint8_t showResponse = 0;

DHT dht(DHTPIN, DHTTYPE, 11); // 11 works fine for ESP8266
                              // Initialize DHT sensor 
                              // NOTE: For working with a faster than ATmega328p 16 MHz Arduino chip, like an ESP8266,
                              // you need to increase the threshold for cycle counts considered a 1 or 0.
                              // You can do this by passing a 3rd parameter for this threshold.  It's a bit
                              // of fiddling to find the right value, but in general the faster the CPU the
                              // higher the value.  The default for a 16mhz AVR is a value of 6.  For an
                              // Arduino Due that runs at 84mhz a value of 30 works.
                              // This is for the ESP8266 processor on ESP-01 
        
float humi, temp;  // Values read from sensor
uint8_t powerevt  = 1;
char tmp[100];
int status = WL_IDLE_STATUS;

void connect(){
   
    while ( status != WL_CONNECTED) {
        Serial.print("Connect to SSID: ");
        Serial.println(ssid);
        // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
        status = WiFi.begin(ssid, password);
        delay(5000);
    } 
  
    Serial.println("Connected to AP");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
    
}

void setup() {
      Serial.begin(115200);
      delay(100);    
      connect();
}
    
void loop() {

  
      status = WiFi.status();
      connect();
      
      Serial.print("connecting to ");
      Serial.println(host);
      
      read_dht22();      
      
      WiFiClient client;
      const int httpPort = 80;
      if (!client.connect(host, httpPort)) {
        Serial.println("connection failed");
        delay_x_min(5);
        return;
      }

      String url = base_url;
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
      
      // Read all the lines of the reply from server, print first and last to Serial
      String line;
      bool isFirst = true;
      uint32_t bytesRead=0;
      Serial.println("Reading Response...");
      
      //this was necessary for connections to a lan server, wan was ok..
      for(int i=0; i < 4; i++){
          if(client.available()) break;
          delay(600);
      }
      
      while(client.available()){
          line = client.readStringUntil('\n');
          if(line.charAt(line.length()) == '\r') line = line.substring(0, line.length()-1);
          bytesRead+= line.length();
          if(showResponse) Serial.print(line); 
          if(isFirst && line.length() > 0){ Serial.println(line); isFirst = false;}
      }
      
      Serial.println(line); 
      Serial.print("Bytes read: "); 
      Serial.println(bytesRead);
      
      Serial.println("closing connection");
      client.stop();
      
      powerevt=0;
      
      delay_x_min(30);
      Serial.println("\r\n\r\n");
      
}
    
void read_dht22() {
     
      humi = dht.readHumidity();          // Read humidity (percent)
      temp = dht.readTemperature(true);   // Read temperature as Fahrenheit
      
      if (isnan(humi) || isnan(temp)) {
          Serial.println("DHT read fail...");
          humi = 66; temp = 66; //probably not hooked up, just use test data...
      }else{
          sprintf(tmp, "Temp=%d   Humi=%d", (int)temp, (int)humi);  
          Serial.println(tmp);
      }

}

void delay_x_min(int minutes){
  
    Serial.print("delay for ");
    Serial.println(minutes);
 
    for(int i=0; i < minutes; i++){
        for(int j=0; j < 240; j++){ //entire j loop = one minute, using small delay so buttons responsive.. 
  	  delay( (speedMode==1 ? 1 : 250) );  
        }
        if(speedMode==0){
            Serial.print(minutes-i);
            Serial.println(" until next upload...");
        }
    }
  
}
