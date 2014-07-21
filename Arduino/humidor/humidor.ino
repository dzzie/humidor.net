#include <dht22.h>
#include <Adafruit_CC3000.h>
#include <ccspi.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_MCP23017.h>
#include <Adafruit_RGBLCDShield.h>
#include <string.h>
#include "utility/debug.h"
#include <stdio.h>
#include "./settings.h"
#include "./private.h"   //rename public.h to private.h and change settings to fit your setup 

bool useTestServerIP = true; //hardcoded in PostData to use 192.168.0.10 set to false to use WebSite

char* WEBSITE = "sandsprite.com";
char* WEBPAGE = "/humidor/logData.php?temp=%d&humi=%d&watered=%d&powerevt=%d&apikey=%s";
int powerevt  = 1;
int watered   = 0;

dht22 DHT22;
Adafruit_RGBLCDShield lcd = Adafruit_RGBLCDShield();

void lcd_out(char*);
void lcd_out(char*, int row);
void delay_x_min(int minutes);
bool ReadSensor(double *temp, double *humi);
bool PostData(int temp, int humi, int watered);

void setup(void)
{
  
  Serial.begin(9600);
  lcd.begin(16, 2);     // set LCD columns and rows
  lcd.setBacklight(WHITE);
  
  DHT22.attach(2);
  Serial.print("DHT22 LIBRARY VERSION: "); Serial.println(DHT22LIB_VERSION);
  
  lcd_out("Init Wifi...");
  Serial.println(F("\nInitializing Wifi..."));
  if (!cc3000.begin())
  {
    lcd_out("Wifi init Failed");
    Serial.println(F("Wifi init failed..Check your wiring?"));
    while(1);
  }

  
}

void loop(void)
{
   double temp, humi;
   char buf[16];
   
   while( !ReadSensor(&temp, &humi) ){
      delay_x_min(1);
      lcd_out("Delaying 1 min",1);
   }
   
   while( !PostData(temp,humi) ){
     delay_x_min(1);
     lcd_out("Delaying 1 min",1);
   }
   
   sprintf(buf, "Temp: %d", (int)temp);
   lcd_out(buf);
   
   sprintf(buf, "Humi: %d", (int)humi);
   lcd_out(buf,1);
     
   /*if(watered){
      lcd.setCursor(15,1); 
      lcd.print("W"); 
   }  
   
   if(powerevt){
      lcd.setCursor(15,0); 
      lcd.print("P"); 
   }*/  
   
   powerevt = 0;
   watered = 0;
   delay_x_min(20);
   
}

void lcd_out(char* s){ lcd_out(s,0); }
  
void lcd_out(char* s, int row){
    if(row==0) lcd.clear();
    lcd.setCursor(0,row); 
    lcd.print(s);
}

void delay_x_min(int minutes){
  char buf[16];  
     
  for(int i=0; i < minutes; i++){
      
      //re-read the sensor every minute to update display? thats allot of sensor reads.. whats its lifetime?
      
      sprintf(buf, "%d min", minutes - i);
      lcd.setCursor(16 - strlen(buf),0); 
      lcd.print(buf);

      for(int j=0; j < 120; j++){ //entire j loop = one minute 
          delay(500);  
          uint8_t buttons = lcd.readButtons();
          if (buttons && (buttons & BUTTON_SELECT) ){
              watered = 1;        
              lcd.setCursor(15,1); 
              lcd.print("W");
          } 
          if(buttons && (buttons & BUTTON_LEFT) ) return; //break delay to do immediate upload test
      }
      
  }
  
}

bool ReadSensor(double *temp, double *humi){
  
  int chk = DHT22.read();
  char* msgs[] = { "DHT22 Bad Checksum", "DHT22 TimeOut", "DHT22 Unknown Error" };
  
  Serial.print("DHT22 Read sensor: ");
  switch (chk)
  {
    case 0: Serial.println("OK"); break;
    case -1: Serial.println(msgs[0]); lcd_out(msgs[0]); return false;
    case -2: Serial.println(msgs[1]); lcd_out(msgs[1]); return false;
    default: Serial.println(msgs[2]); lcd_out(msgs[2]); return false;
  }

  *humi = DHT22.humidity;
  *temp = DHT22.fahrenheit();
  return true;
  
}


bool PostData(int temp, int humi)
{
  int MAX_TICKS = 1000;
  int ticks = 0;
  uint32_t ip;
  char buf[200];
  
  lcd_out("Connecting AP");
  Serial.print(F("\nAttempting to connect to ")); Serial.println(WLAN_SSID);
  if (!cc3000.connectToAP(WLAN_SSID, WLAN_PASS, WLAN_SECURITY)) {
    Serial.println(F("Failed to connect to AP"));
    lcd_out("No Ap Connect",1);
    return false;
  }
   
  Serial.println(F("Connected!"));
  
  /* Wait for DHCP to complete */
  Serial.println(F("Request DHCP"));
  lcd_out("DHCP REQ...");
  while (!cc3000.checkDHCP())
  {
    delay(100); 
    ticks++;
    if(ticks > MAX_TICKS) return false;
  }  

  ticks = 0;
  while ( !displayConnectionDetails() ) 
  { /* Display the IP address DNS, Gateway, etc. */
    delay(1000);
    ticks++;
    if(ticks > MAX_TICKS) return false;
  }

  ip = 0;
  
  if(useTestServerIP){
      ip = cc3000.IP2U32(192,168,0,10); //direct ip connection..
  }
  else{
      // Try looking up the website's IP address
      Serial.print(WEBSITE); Serial.print(F(" -> "));
      lcd_out("GetHostName");
      lcd_out(WEBSITE, 1);
      
      ticks = 0;
      while (ip == 0) {
          if (! cc3000.getHostByName(WEBSITE, &ip)) {
            Serial.println(F("Couldn't resolve!"));
            lcd_out("GetHost Failed!", 0);
          }
          delay(500);
          ticks++;
          if(ticks > MAX_TICKS) return false;
      }
  }

  cc3000.printIPdotsRev(ip);
  Adafruit_CC3000_Client www = cc3000.connectTCP(ip, 80);
  
  sprintf(buf, WEBPAGE, temp, humi, watered, powerevt, APIKEY);
    
  if( www.connected() ) {
    lcd_out("Submitting Data!");
    www.fastrprint(F("GET "));
    www.fastrprint(buf);
    www.fastrprint(F(" HTTP/1.1\r\n"));
    www.fastrprint(F("Host: ")); www.fastrprint(WEBSITE); www.fastrprint(F("\r\n"));
    www.fastrprint(F("\r\n"));
    www.println();
  } 
  else{
    lcd_out("Website Down?", 1);
    Serial.println(F("Web Connection failed"));    
    return false;
  }
 
  /* Read data until either the connection is closed, or the idle timeout is reached. */ 
  unsigned long lastRead = millis();
  while (www.connected() && (millis() - lastRead < IDLE_TIMEOUT_MS)) {
    while (www.available()) {
      char c = www.read();
      Serial.print(c);
      lastRead = millis();
    }
  }
  www.close();
  
  /* You need to make sure to clean up after yourself or the CC3000 can freak out next connect */
  Serial.println(F("\n\nWeb Send Complete"));
  cc3000.disconnect();
  return true;
  
}




bool displayConnectionDetails(void)
{
  uint32_t ipAddress, netmask, gateway, dhcpserv, dnsserv;
  
  if(!cc3000.getIPAddress(&ipAddress, &netmask, &gateway, &dhcpserv, &dnsserv))
  {
    lcd_out("No IP", 1);
    Serial.println(F("Unable to retrieve the IP Address!\r\n"));
    return false;
  }
  else
  {
    Serial.print(F("\nIP Addr: ")); cc3000.printIPdotsRev(ipAddress);
    Serial.print(F("\nNetmask: ")); cc3000.printIPdotsRev(netmask);
    Serial.print(F("\nGateway: ")); cc3000.printIPdotsRev(gateway);
    Serial.print(F("\nDHCPsrv: ")); cc3000.printIPdotsRev(dhcpserv);
    Serial.print(F("\nDNSserv: ")); cc3000.printIPdotsRev(dnsserv);
    Serial.println();
    return true;
  }
}
