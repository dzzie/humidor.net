
#include <Adafruit_CC3000.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_RGBLCDShield.h>
#include <string.h>
#include <stdio.h>
#include "./settings.h"
#include "./private.h"   //rename public.h to private.h and change settings to fit your setup 

//switched to arduino 1.5.x because it optimizes better and saves us more space (were pretty full up for an uno!)

//note disabled serial.printlns to save space, seems to get buggy over 28k sketch size?

 
#define sensorCount 2

int pins[sensorCount] = {2,6}; //free digital pins for dht22(s): 2,6,7,8
double temp[sensorCount] = {0,0};
double humi[sensorCount] = {0,0};
double last_temp[sensorCount] = {0,0};
double last_humi[sensorCount] = {0,0};

#define debug           1                 /*use test server, no dht22 required*/
#define test_server_ip  "192.168.0.10" 
#define live_server_ip  "67.210.116.230" 
#define DOMAIN          "sandsprite.com"       
#define WEBPAGE         "/humidor/logData.php?" 
#define firmware_ver    "v1.2 " __DATE__  

uint32_t ip;
uint8_t powerevt  = 1;
uint8_t watered   = 0;
uint8_t smoked    = 0;
uint8_t failure   = 0;
uint8_t inReadSensor  = 0;

char buf[150];
char tmp[55];

Adafruit_RGBLCDShield lcd = Adafruit_RGBLCDShield();

//following will allow you to add conditional compilation of serial debugging code 
//with single line statements. saves space when disabled..
/*#ifdef WITH_SERIAL
   #define IFS(x) x
#else
   #define IFS(x)
#endif*/

void setup(void)
{
  
  Serial.begin(9600);
  lcd.begin(16, 2);     // set LCD columns and rows
  lcd.setBacklight(WHITE);
  
  lcd_out("Daves WebHumidor");
  lcd_out(firmware_ver,1);
  delay(2000);  

  lcd_out("Init Wifi...");
  if (!cc3000.begin()){
	  lcd_out("Fail!",1);
	  while(1);
  }
  
  if(debug){
		ip = ips2ip(test_server_ip);
  }else{
		ip = ips2ip(live_server_ip);
  }
 
  /*
  unsigned long aucDHCP = 14400;
  unsigned long aucARP = 3600;
  unsigned long aucKeepalive = 10;
  unsigned long aucInactivity = 20;
  
  if (netapp_timeout_values(&aucDHCP, &aucARP, &aucKeepalive, &aucInactivity) != 0) {
    //IFS( Serial.println("Error setting inactivity timeout!"); )
  }*/

  
}

void loop(void)
{

   int fail_cnt = 0;

   if(debug){
	   for(uint8_t i=0; i < sensorCount; i++){temp[i] = 66; humi[i] = 66;}
   }else{
	   for(uint8_t i=0; i < sensorCount; i++){
		   for(fail_cnt=0; fail_cnt < 10; fail_cnt++){
				if( ReadSensor(i) ) break;
				lcd_out("Delaying 1 min",1);
				delay_x_min(1,1);
		   }
		   if(fail_cnt >= 10){
				failure = 1;
				PostData();
				lcd_out("DHT22 FailMode");
				while(1);
		   }
	   }
   }

   show_readings(0);
   delay(2500); //time to see immediate readings when I hit the button before submit..
   fail_cnt = 0;

   while( !PostData() ){
     lcd_out("Delaying 1 min",1);
     delay_x_min(1,1);
	 fail_cnt++;
   }
   
   show_readings(0);
   
   if(fail_cnt > 2){
		sprintf(tmp, "%d Fails", fail_cnt);
        lcd.setCursor(15-strlen(tmp),1); 
		lcd.print(tmp);
   }

   powerevt = 0;
   watered = 0;
   smoked = 0;
   delay_x_min(30); //todo cycle through sensor readings if multiple during delay..
   
}

void show_readings(int i){
     sprintf(tmp, "Temp: %d      %d", (int)temp[i], i);
     lcd_out(tmp);
   
     sprintf(tmp, "Humi: %d", (int)humi[i]);
     lcd_out(tmp,1);
}



void delay_x_min(int minutes){
    delay_x_min(minutes, 0);
}

void delay_x_min(uint8_t minutes, uint8_t silent){

  for(uint8_t i=0; i < minutes; i++){
      
      //re-read the sensor every minute to update display? thats allot of sensor reads.. whats its lifetime?
      
      if(silent==0){
          sprintf(tmp, " %d min", minutes - i); //fixed display bug lcd not overwriting tens digit once <= 9
          lcd.setCursor(16 - strlen(tmp),0); 
          lcd.print(tmp);
      }

      for(uint8_t j=0; j < 240; j++){ //entire j loop = one minute, using small delay so buttons responsive.. 
          delay(250);  
          uint8_t buttons = lcd.readButtons();
          if (buttons && (buttons & BUTTON_SELECT) ){
              watered = 1;        
              lcd.setCursor(15,1); 
              lcd.print("W");
          } 
		  if (buttons && (buttons & BUTTON_DOWN) ){
              smoked = 1;        
              lcd.setCursor(14,1); 
              lcd.print("S");
          } 
          if(buttons && (buttons & BUTTON_LEFT) ) return; //break delay to do immediate upload test
      }
      
  }
  
}

bool ReadSensor(uint8_t i /* sensorIndex */){
  
  uint8_t chk = dht22_read(i); //0= OK, -1 = Bad Chksum, -2 = Time Out

  if (chk != 0){
	  sprintf(tmp,"DHT22 %d Fail: %d", i, chk);
	  lcd_out(tmp); 
	  delay(1200);
	  return false;
  }

  humi[i] += humi_shift;
  temp[i] += temp_shift;

  if(last_humi[i] == 0) last_humi[i] = humi[i];
  if(last_temp[i] == 0) last_temp[i] = temp[i];

  uint8_t delta_h = abs((int)last_humi[i] - (int)humi[i]);
  uint8_t delta_t = abs((int)last_temp[i] - (int)temp[i]);

  //we may have gotten a bad sensor read as it varied more than expected from last..
  //we will try again up to three more times with recursive call endless loop protection..
  //if we still get an unexpected reading after all of this we will accept it
  if(delta_h > 3 || delta_t > 5){
       
	   if(inReadSensor) return false; //delta still high, in recursive call, exit with fail..

	   inReadSensor = 1;
	   for(int i=1; i <=3; i++){ if( ReadSensor(i) ) break; } //only done for top level call
	   inReadSensor = 0;
  }

  last_humi[i] = humi[i];
  last_temp[i] = temp[i];

  return true;
  
}


bool PostData()
{
  #define MAX_TICKS 1000

  int ticks = 0;

  //these are for our http parser, since we dont want to store the response in a buffer to parse latter
  //we will parse it on the fly as its received character by character (limited memory, was glitching other way)
  uint8_t nl_count = 0;
  int rc_offset = 0;
  int pr_offset = 0;
  uint8_t recording = 0;
  char respCode[18];
  char pageResp[18];

  unsigned long lastRead = 0;
  
  Adafruit_CC3000_Client www;

  lcd_out("AP Connect");
  if (!cc3000.connectToAP(WLAN_SSID, WLAN_PASS, WLAN_SECURITY)) return false;
 
  lcd_out("DHCP");
  for(ticks=0; ticks < MAX_TICKS; ticks++)
  {
    if( cc3000.checkDHCP() ) break;
    delay(100); 
  }  

  if(ticks >= MAX_TICKS) goto EXIT_FAIL;

  lcd_out("Submit");
  lcd_ip_out(ip,1);  
  delay(2000);
    
  www = cc3000.connectTCP(ip, 80); //have been having occasional hang here...
 
  //breaking this up so no one sprintf takes up to much memory..
  strcpy(buf, WEBPAGE);
  sprintf(tmp, "watered=%d&powerevt=%d&cnt=%d", watered, powerevt, sensorCount);
  strcat(buf,tmp);

  for(uint8_t i=0; i < sensorCount; i++){
		sprintf(tmp,"&temp%d=%d&humi%d=%d", i, (int)temp[i], i, (int)humi[i] ); //these int casts are mandatory or corruption occurs
		lcd_out(tmp);
		delay(1200);
		if(strlen(buf)+strlen(tmp) < sizeof(buf) ){
			strcat(buf, tmp);
		}else{
			sprintf(tmp, "%d bytes > %d", strlen(buf) + strlen(tmp), sizeof(buf) );
			lcd_out("DEV BOF STOP");
			lcd_out(tmp, 1);
			while(1);
		}
  }

  sprintf(tmp, "&failure=%d&clientid=%d&smoked=%d&apikey=", failure, client_id, smoked);
  strcat(buf,tmp);
  strcat(buf,APIKEY);

  if ( !cc3000.checkConnected() ) goto EXIT_FAIL;
  
  sprintf(tmp, "%d bytes     ", strlen(buf) );
  lcd_out(tmp,1); 
  delay(1200);

  if( www.connected() ) {
    www.fastrprint(F("GET "));
    www.fastrprint(buf);
    www.fastrprint(F(" HTTP/1.1\r\n"));
    www.fastrprint(F("Host: ")); www.fastrprint(DOMAIN); www.fastrprint(F("\r\n"));
    www.fastrprint(F("\r\n"));
    if ( !cc3000.checkConnected() ) goto EXIT_FAIL;
    www.println();
  } 
  else{
    //lcd_out("Website Down?", 1);
    //delay(800); 
    goto EXIT_FAIL;
  }
  
  lcd_out("Reading Response");
  lastRead = millis();

   /* Read data until either the connection is closed, or the idle timeout is reached. */ 
  while (www.connected()) {
    
    //this apparently happens every time.. can not use as a marker..
    if( (millis() - lastRead) > IDLE_TIMEOUT_MS ) break;
    
    while (www.available()) {
        char c = www.read();
        //IFS( Serial.print(c); )
      
        //this accounts for \r\n or just \n, (debugged in visual studio..)
        //we do a crude parse of the http headers to extract response code and first 
        //16chars of web page response for display on lcd for visual confirmation..
        if(c == 0x0A){ 
            nl_count += 1; 
			recording=0;
		}else{ 
			if(c != 0x0D) nl_count = 0; 
		}

        if(c == 0x20 && rc_offset == 0 && nl_count==0){
             recording = 1;
        }else{
            if(recording == 1 && rc_offset >= 16) recording = 0;
  			if(recording == 1 && c == 0x0D) recording = 0;
            if(recording == 1) respCode[rc_offset++] = c;
        }
        
		if(nl_count==2 && pr_offset==0){
			recording = 2; //end of http headers..turn copy on, starts next iter..
		}else{
			if(recording == 2 && pr_offset >= 16) recording = 0;
			if(recording == 2 && c == 0x0D) recording = 0;
			if(recording == 2) pageResp[pr_offset++] = c; 
		}
		
		lastRead = millis();
	}
  }
  www.close();
  
  pageResp[pr_offset]=0;
  respCode[rc_offset]=0;
  //int rCode = atoi(respCode); //this is the 404 for not found, or 200 for ok as numeric..
  if(rc_offset) lcd_out(respCode); else lcd_out("Bad RC ");
  if(pr_offset) lcd_out(pageResp,1); else lcd_out("Bad PR ",1);
  
  delay(2500);
  
  cc3000.disconnect(); /* must clean up or CC3000 can freak out next connect */
  return true;
  
EXIT_FAIL:
   cc3000.disconnect(); 

   lcd_out("Exit Fail?", 1);
   delay(800); 

   return false;
  

}


void lcd_out(char* s){ lcd_out(s,0); }
  
void lcd_out(char* s, uint8_t row){
    if(row==0) lcd.clear();
    lcd.setCursor(0,row); 
    lcd.print(s);
}

void lcd_ip_out(uint32_t ip, uint8_t row){
  uint8_t* b = (uint8_t*)&ip;
  sprintf(tmp,"%d.%d.%d.%d", b[3], b[2], b[1], b[0] );
  lcd_out(tmp, row);
}

//ip string to ipu32 -  usage:  uint32_t ip = ips2ip("192.168.0.10");
uint32_t ips2ip(char* ips){
  uint8_t i=0, j=0;
  uint8_t b[4]={0,0,0,0};
  char tmp[5]={0,0,0,0,0};

  while(*ips){
      if(*ips=='.'){
          b[j++] = atoi(tmp);
          for(;i>=0; i--) tmp[i]=0; //wipe tmp for next
          i=0;
          if(j == 4) break; //max 4 sections
      }else{
         tmp[i++] = *ips;
         if( i > 4 ) break; //max 3 chars per sect
      }
      ips++;
  }

  if(tmp[0] != 0) b[j++] = atoi(tmp);
  if(j != 4) return 0; //need exactly 4 sections
  return cc3000.IP2U32(b[0], b[1], b[2], b[3]);

}



/*  - extracted from DHT22 library and put inline to save space -
	FILE: dht22.cpp - Library for the Virtuabotix DHT22 Sensor.
	VERSION: 1S0A
	PURPOSE: Measure and return temperature & Humidity. Additionally provides conversions.
	LICENSE: GPL v3 (http://www.gnu.org/licenses/gpl.html)
	Joseph Dattilo (Virtuabotix LLC) - Version 1S0A (14 Sep 12)
*/
double toFahrenheit(double dCelsius)
{
	return 1.8 * dCelsius + 32;
}

int dht22_read(uint8_t sensorIndex)
{
	if(sensorIndex > sensorCount) return 0;
	
	int pin = pins[sensorIndex];

	// BUFFER TO RECEIVE
	uint8_t bits[5];
	uint8_t cnt = 7;
	uint8_t idx = 0;

	// EMPTY BUFFER
	for (uint8_t i=0; i< 5; i++) bits[i] = 0;

	// REQUEST SAMPLE
	pinMode(pin, OUTPUT);
	digitalWrite(pin, LOW);
	delay(18);
	digitalWrite(pin, HIGH);
	delayMicroseconds(40);
	pinMode(pin, INPUT);

	// ACKNOWLEDGE or TIMEOUT
	unsigned int loopCnt = 10000;
	while(digitalRead(pin) == LOW)
		if (loopCnt-- == 0) return -2;

	loopCnt = 10000;
	while(digitalRead(pin) == HIGH)
		if (loopCnt-- == 0) return -2;

	// READ OUTPUT - 40 BITS => 5 BYTES or TIMEOUT
	for (uint8_t i=0; i<40; i++)
	{
		loopCnt = 10000;
		while(digitalRead(pin) == LOW)
			if (loopCnt-- == 0) return -2;

		unsigned long t = micros();

		loopCnt = 10000;
		while(digitalRead(pin) == HIGH)
			if (loopCnt-- == 0) return -2;

		if ((micros() - t) > 40) bits[idx] |= (1 << cnt);
		if (cnt == 0)   // next byte?
		{
			cnt = 7;    // restart at MSB
			idx++;      // next byte!
		}
		else cnt--;
	}

	// WRITE TO RIGHT VARS
	humi[sensorIndex] = word(bits[0], bits[1]) *.1;//calculates and stores the humidity

	 uint8_t sign = 1;
     if (bits[2] & 0x80) // negative temperature
     {
            bits[2] = bits[2] & 0x7F;//negative temp adjustments
            sign = -1;
     }
    
	temp[sensorIndex] = toFahrenheit(sign * word(bits[2], bits[3]) * 0.1);//temp calculation and storage

	uint8_t sum = bits[0] + bits[1] + bits[2] + bits[3];//sum values

	if (bits[4] != sum) return -1;//failed checksum
	return 0;//great success!
}


/*
bool displayConnectionDetails(void)
{
  uint32_t ipAddress, netmask, gateway, dhcpserv, dnsserv;
  
  if(!cc3000.getIPAddress(&ipAddress, &netmask, &gateway, &dhcpserv, &dnsserv))
  {
    lcd_out("No IP", 1);
    //Serial.println(F("Unable to retrieve the IP Address!\r\n"));
    return false;
  }
  
  /*
  Serial.print(F("\nIP Addr: ")); cc3000.printIPdotsRev(ipAddress);
  Serial.print(F("\nNetmask: ")); cc3000.printIPdotsRev(netmask);
  Serial.print(F("\nGateway: ")); cc3000.printIPdotsRev(gateway);
  Serial.print(F("\nDHCPsrv: ")); cc3000.printIPdotsRev(dhcpserv);
  Serial.print(F("\nDNSserv: ")); cc3000.printIPdotsRev(dnsserv);
  Serial.println();
  * /
  
  return true;
  
}
*/
