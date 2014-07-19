#include <dht22.h>
#include <Adafruit_CC3000.h>
#include <ccspi.h>
#include <SPI.h>
#include <string.h>
#include "utility/debug.h"
#include <stdio.h>
#include "./settings.h"
#include "./private.h"   //rename public.h to private.h and change settings to fit your setup 

#define WEBSITE      "sandsprite.com"
#define WEBPAGE      "/humidor/logData.php?temp=%d&humi=%d&watered=%d&apikey=%s"

dht22 DHT22;

void setup(void)
{
  
  Serial.begin(115200);
  
  DHT22.attach(2);
  Serial.print("DHT22 LIBRARY VERSION: "); Serial.println(DHT22LIB_VERSION);
  
  Serial.println(F("\nInitializing Wifi..."));
  if (!cc3000.begin())
  {
    Serial.println(F("Wifi init failed..Check your wiring?"));
    while(1);
  }

  
}

void loop(void)
{
   double temp, humi;
   
}

void delay_x_min(int minutes){
  for(int i=0; i < minutes; i++){
     delay(1000 * 60);  
  }
}

bool ReadSensor(double *temp, double *humi){
  
  int chk = DHT22.read();

  Serial.print("Read sensor: ");
  switch (chk)
  {
    case 0: Serial.println("OK"); break;
    case -1: Serial.println("DHT22 Bad Checksum"); return false;
    case -2: Serial.println("DHT22 TimeOut"); return false;
    default: Serial.println("DHT22 Unknown Error");  return false;
  }

  *humi = DHT22.humidity;
  *temp = DHT22.fahrenheit();
  return true;
  
}


bool PostData(int temp, int humi, int watered)
{
  int MAX_TICKS = 1000;
  int ticks = 0;
  uint32_t ip;
      
  Serial.print(F("\nAttempting to connect to ")); Serial.println(WLAN_SSID);
  if (!cc3000.connectToAP(WLAN_SSID, WLAN_PASS, WLAN_SECURITY)) {
    Serial.println(F("Failed to connect to AP"));
    return false;
  }
   
  Serial.println(F("Connected!"));
  
  /* Wait for DHCP to complete */
  Serial.println(F("Request DHCP"));
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
  ip = cc3000.IP2U32(192,168,0,10); //direct ip connection..
  
  /* Try looking up the website's IP address
  Serial.print(WEBSITE); Serial.print(F(" -> "));
  while (ip == 0) {
    if (! cc3000.getHostByName(WEBSITE, &ip)) {
      Serial.println(F("Couldn't resolve!"));
    }
    delay(500);
  }*/

  cc3000.printIPdotsRev(ip);
  Adafruit_CC3000_Client www = cc3000.connectTCP(ip, 80);
  
  char buf[200];
  sprintf(buf, WEBPAGE, temp, humi, watered, APIKEY);
  
  if( www.connected() ) {
    www.fastrprint(F("GET "));
    www.fastrprint(buf);
    www.fastrprint(F(" HTTP/1.1\r\n"));
    www.fastrprint(F("Host: ")); www.fastrprint(WEBSITE); www.fastrprint(F("\r\n"));
    www.fastrprint(F("\r\n"));
    www.println();
  } 
  else{
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
