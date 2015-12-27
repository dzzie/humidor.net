/*
Copyright David Zimmer <dzzie@yahoo.com>
WebSite:  http://sandsprite.com
All rights reserved, no portion of this code is authorized for sale or redistribution

ini debug_local = 1 -> use test server, no dht22 required

new ini library saves ~500 bytes of both code and ram, and much easier to read...

bugnote: apparently www.fastrprint will hang on to long of a string passed to it..
         this may be why i needed a watchdog in the original version?

 NOTE: compile this with arduino ide 1.5.8

 Binary sketch size: 52,678 bytes (used 21% of a 253,952 byte maximum) (3.05 secs)
Minimum Memory Usage: 2601 bytes (32% of a 8192 byte maximum)

*/

#include <SPI.h>
#include <Wire.h>
#include <string.h>
#include <stdio.h>
#include <avr/wdt.h>
#include <Adafruit_GFX.h>   
#include <Adafruit_ILI9341.h>
#include <Adafruit_FT6206.h>
#include <SPI.h>
#include <Adafruit_CC3000.h>
#include <SD.h>
#include "ini.h"

//so apparently if you send serial data without a pc attached it can bug out the arduino.
//I found a couple references online specifically when using I2C devices in conjunction..
#define WITH_SERIAL 0

// The FT6206 uses hardware I2C (SCL/SDA)
Adafruit_FT6206 ctp = Adafruit_FT6206();

#define TFT_CS 10
#define TFT_DC 9
#define SD_CS 4
Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC);

#define dht22_pin     46
#define EXT_WATCHDOG_PIN 44

//IRQ MUST be an interrupt pin!
//note: the CC3000 IRQQ pin had to be moved from 21 since it was causing lockups
//      with the touch screen. There was a work around of calling cc3000.stop() and reboot,
//      but it was not stable and causing problems.. IRQ on pin 19 seems ok so far..
//      just note the hardware pictures may be wrong
#define ADAFRUIT_CC3000_IRQ   19  
#define ADAFRUIT_CC3000_VBAT  49
#define ADAFRUIT_CC3000_CS    48
Adafruit_CC3000 cc3000 = Adafruit_CC3000(ADAFRUIT_CC3000_CS, ADAFRUIT_CC3000_IRQ, ADAFRUIT_CC3000_VBAT, SPI_CLOCK_DIVIDER); 
unsigned long IDLE_TIMEOUT_MS = 7000; // Amount of time to wait (in milliseconds) with no data (7 seconds)

char* WEBPAGE = "/humidor/logData.php"; 
#define firmware_ver  "v1.3 " __DATE__  
#define MAX_TICKS 1000

char *DFAIL = "DHT22 Fail";

volatile int counter;      // Count number of times ISR is called.
volatile int countmax = 8; // Timer expires after about 64 secs (using 8 sec interval)

uint8_t powerevt  = 1;
uint8_t watered   = 0;
uint8_t smoked    = 0;
uint8_t failure   = 0;
uint8_t inReadSensor  = 0;
int uploads  = 0;
int fail_cnt = 0;
int curTab = 0;

char tmp[100] = {0};

double temp      = 0;
double humi      = 0;
double last_temp = 0;
double last_humi = 0;

struct CONFIG{
	int temp_shift;   
	int humi_shift;
	int client_id;
	int ext_watchdog;
	int debug_local;
	int demoMode;
	int speedMode;
	char* apikey;      //apikey for web app user specified by client_id
	char* server;      //domain name of production server (lookup + hostname use)
	char* test_ip;     //local test server ip (debug_local must be 1 to use)
	char* ssid;        // cannot be longer than 32 characters!
	char* security;    // Security can be WLAN_SEC_UNSEC, WLAN_SEC_WEP, WLAN_SEC_WPA or WLAN_SEC_WPA2
	char* pass;        //wifi password, if wep it will be transformed to binary from hex codes..
	uint32_t activeIP; //filled out at runtime from string either dns lookup or from ip string..
	uint8_t secMode;
};


char* ssid = "ssid";
char* security = "security";
char* pass = "pass";
char* ext_watchdog = "ext_watchdog";
char* temp_shift = "temp_shift";
char* humi_shift = "humi_shift";
char* debug_local = "debug_local";
char* demoMode = "demoMode";
char* speedMode = "speedMode";
char* client_id = "client_id";
char* apikey = "apikey";
char* server = "server";
char* test_ip = "test_ip";
char* missing = "Missing ";
char* stars = "*******";
char* iniFile = "private.ini";


CONFIG cfg;
//CFG_STRINGS sCfg;

//following will allow you to add conditional compilation of serial debugging code 
//with single line statements. saves space when disabled..
/*#ifdef WITH_SERIAL
   #define IFS(x) x
#else
   #define IFS(x)
#endif*/

void cls(){
	tft.fillScreen(ILI9341_BLACK);
    tft.setCursor(0,0);
}

void setup(void)
{
  
  if(WITH_SERIAL){
	  Serial.begin(9600); 
	  while(!Serial);
	  Serial.println("Init tft..");
  }

  tft.begin();
  tft.setTextSize(2);

  cls();
  lcd_outp(F("Daves WebHumidor"));
  lcd_out(firmware_ver,1);

  lcd_out("Init touch..");

  if ( !ctp.begin(40) ) {  // pass in 'sensitivity' coefficient
    lcd_out("Failed..");
    while (1);
  }
  
  lcd_out("Init Wifi...");
  if (!cc3000.begin()){
    lcd_out("Failed!"); 
	while(1);
  }
  
  loadConfig();
  
  if(cfg.debug_local==0 || cfg.demoMode==1){
	  tft.fillScreen(ILI9341_WHITE);
	  bmpDraw("sir.bmp", 0, 0);
	  delay(1500);
	  tft.fillScreen(ILI9341_BLACK);
  }

}

void loop(void)
{

   if(cfg.debug_local){
       temp = 66; humi = 66;
   }else{
	   cls();
	   lcd_outp(F("Reading sensor"),1);
	   for(int i=0; i <= 10; i++){
			if( ReadSensor() ) break;
			delay(1000);
			if(i == 10){
				failure = 1;
				watchdogEnable();
				PostData();
				watchdogDisable();
				lcd_out(DFAIL);
				while(1);
		   }
       }
   }

   show_readings(false);
   delay(2500); //time to see immediate readings when I hit the button before submit..
   watchdogEnable();
   
   if( PostData() ){
	   fail_cnt = 0;
	   watered = 0;
	   smoked = 0;
	   powerevt = 0;
   }
   else{
		lcd_outp(F("Upload failed"),1);
		delay(1200);
		fail_cnt++;
		if(fail_cnt > 2) cfg.activeIP = 0;
   }

   watchdogDisable();
   show_readings(true);
   delay_x_min( (fail_cnt == 0 ? 30 : 5) ); //webui expects 30min delay for stat gen

}

void watchdogReset()
{
	if(cfg.ext_watchdog==1){
		pinMode(EXT_WATCHDOG_PIN, OUTPUT);
		delay(200);
		pinMode(EXT_WATCHDOG_PIN, INPUT);
	}else
		wdt_reset();
}

void watchdogDisable()
{
	if(cfg.ext_watchdog==1)
		pinMode(EXT_WATCHDOG_PIN, OUTPUT);
	else
		wdt_disable();

}

//updated code to extend watch dog to 24 seconds.
//special thanks to Philip Allagas for bringing this post to my attention
//and Dave Evans for the code :)
//http://forum.arduino.cc/index.php?topic=248263.0 

void watchdogEnable()
{
  if(cfg.ext_watchdog==1)
		watchdogReset();
  else{

	 counter=0;
	 wdt_reset();
	 cli();                              // disable interrupts

	 MCUSR = 0;                          // reset status register flags

										 // Put timer in interrupt-only mode:                                        
	 WDTCSR |= 0b00011000;               // Set WDCE (5th from left) and WDE (4th from left) to enter config mode,
										 // using bitwise OR assignment (leaves other bits unchanged).
	 WDTCSR =  0b01000000 | 0b100001;    // set WDIE (interrupt enable...7th from left, on left side of bar)
										 // clr WDE (reset enable...4th from left)
										 // and set delay interval (right side of bar) to 8 seconds,
										 // using bitwise OR operator.

	 sei();                              // re-enable interrupts
	 //wdt_reset();                      // this is not needed...timer starts without it

	 // delay interval patterns:
	 //  16 ms:     0b000000
	 //  500 ms:    0b000101
	 //  1 second:  0b000110
	 //  2 seconds: 0b000111
	 //  4 seconds: 0b100000
	 //  8 seconds: 0b100001
  }

}

ISR(WDT_vect) // watchdog timer interrupt service routine
{
		 counter+=1;

		 if (counter < countmax)
		 {
		   wdt_reset(); // start timer again (still in interrupt-only mode)
		 }
		 else             // then change timer to reset-only mode with short (16 ms) fuse
		 {
		   
		   MCUSR = 0;                          // reset flags

											   // Put timer in reset-only mode:
		   WDTCSR |= 0b00011000;               // Enter config mode.
		   WDTCSR =  0b00001000 | 0b000000;    // clr WDIE (interrupt enable...7th from left)
											   // set WDE (reset enable...4th from left), and set delay interval
											   // reset system in 16 ms...
											   // unless wdt_disable() in loop() is reached first

		   //wdt_reset(); // not needed
		 }
}

void show_readings(bool drawButtons){
     
	  uint16_t hColor = ILI9341_GREEN;
	  uint16_t tColor = ILI9341_GREEN;

	  if((int)temp < 60 || (int)temp > 75) tColor = ILI9341_RED;
	  if((int)humi < 60 || (int)humi > 75) hColor = ILI9341_RED;

	  cls();
  
	  tft.setTextSize(4);
	  tft.print("Temp:");

	  tft.setTextColor(tColor);
	  tft.setTextSize(9);
	  tft.print((int)temp);
	  
	  tft.setTextSize(2);
	  tft.println();
	  
	  tft.setTextColor(ILI9341_WHITE);
	  tft.setTextSize(4);
	  tft.print("Humi:");

	  tft.setTextColor(hColor);
	  tft.setTextSize(9);
	  tft.print((int)humi);

	  tft.setTextColor(ILI9341_WHITE);
	  tft.setTextSize(2);

	  if(fail_cnt > 2){
			sprintf_P(tmp, PSTR("\n\n\n%d Fails"), fail_cnt);
	  }
	  else{//track # successful uploads since last reset (watch watchdog)
			sprintf(tmp, "\n\n\n%d Uploads", uploads);
	  }

	  tft.println(tmp);

	  if(drawButtons){
		  bmpDraw("water.bmp", 180, 280); //240w x 320h screen
		  bmpDraw("cigar.bmp", 20, 280); //43w x 33h   images
		  bmpDraw("config.bmp", 100, 280);
	  }

	  //IFS(Serial.print("X:");Serial.println(tft.getCursorX());)
}

void lcd_out(char* s){ lcd_out(s,0); }
void lcd_outp(const __FlashStringHelper *s){lcd_outp(s,0); }
void delay_x_min(int minutes){ delay_x_min(minutes, 0); } 
void lcd_out(char* s, int row){	tft.println(s); }

void lcd_outp(const __FlashStringHelper *s, int row){
    uint8_t c;
	const char PROGMEM *p = (const char PROGMEM *)s;
    while ((c = pgm_read_byte_near(p++)) != 0) tft.print((char)c);
	tft.print('\n');
}

void displayFlags(int min){
	tft.setTextSize(3);
	sprintf_P(tmp, PSTR("%d min   "), min); //fixed display bug lcd not overwriting tens digit once <= 9
	tft.fillRect(0, tft.width() - 7, tft.width(), 30 ,ILI9341_BLACK);
	tft.setCursor(0, tft.width() - 7);
	tft.print(tmp);
	if(watered) tft.print('W'); 
    if(smoked)  tft.print('S'); 
	tft.setTextSize(2);
}

void delay_x_min(int minutes, int silent){

  for(int i=0; i < minutes; i++){
      
      //re-read the sensor every minute to update display? thats allot of sensor reads.. whats its lifetime?
      
      if(silent==0) displayFlags( minutes - i );

      for(int j=0; j < 240; j++){ //entire j loop = one minute, using small delay so buttons responsive.. 
		  delay( (cfg.speedMode==1 ? 1 : 250) );  

		  if ( ctp.touched() ) {
			    TS_Point p = ctp.getPoint(); 
				//sprintf(tmp,"\nx:%d\ny:%d", p.x, p.y);
				//lcd_out(tmp);

				if( p.y < 50){ //its in the lower image bar band.. |__| <-0,0
					if(p.x > 140){ //its on the left hand cigar half
						smoked = smoked == 1 ? 0 : 1;      
					}else if(p.x > 60){
						if(WITH_SERIAL) Serial.println("enter config!");
						cls();
						showCfg(true); //timer stops until they close screen..
						cls();
						show_readings(true);
						displayFlags( minutes - i );
					}else{
						watered = watered == 1 ? 0 : 1;  
					}
					displayFlags(minutes - i);
				}

				delay(500); //software debounce
		  }

          /*
		  if (buttons && (buttons & BUTTON_UP) ){
			  if( ReadSensor() ) show_readings(); else lcd_outp(F("Read Fail?"));
		  }
		  if (buttons && (buttons & BUTTON_RIGHT) ){ //speed up clock to test 6x pump delay..
	          speedMode = speedMode ==1 ? 0 : 1
			  delay(500); //debounce
		  }	  
          if(buttons && (buttons & BUTTON_LEFT) ) return; //break delay to do immediate upload test
		  */
      }
       
  }
  
}

bool ReadSensor(){
  
  int chk = dht22_read(dht22_pin); //OK: 0, Bad Chksum: -1, Time Out: -2

  if (chk != 0){
	  lcd_out(DFAIL); 
	  delay(1200);
	  return false;
  }

  humi += cfg.humi_shift;
  temp += cfg.temp_shift;

  if(last_humi == 0) last_humi = humi;
  if(last_temp == 0) last_temp = temp;

  int delta_h = abs((int)last_humi - (int)humi);
  int delta_t = abs((int)last_temp - (int)temp);

  //we may have gotten a bad sensor read as it varied more than expected from last..
  //we will try again up to three more times with recursive call endless loop protection..
  //if we still get an unexpected reading after all of this we will accept it
  if(delta_h > 3 || delta_t > 5){
       
	   if(inReadSensor) return false; //delta still high, in recursive call, exit with fail..

	   inReadSensor = 1;
	   for(int i=1; i <=3; i++){ if( ReadSensor() ) break; } //only done for top level call
	   inReadSensor = 0;
  }

  last_humi = humi;
  last_temp = temp;

  return true;
  
}


unsigned long timeDiff(unsigned long startTime){
	return millis() - startTime;
}

void lcd_ip_out(uint32_t ip){
  uint8_t* b = (uint8_t*)&ip;
  sprintf_P(tmp,PSTR("%d.%d.%d.%d"), b[3], b[2], b[1], b[0] );
  tft.println(tmp);
}

bool PostData()
{
  char buf[500];
  int ticks = 0;

  //these are for our http parser, since we dont want to store the response in a buffer to parse latter
  //we will parse it on the fly as its received character by character (limited memory, was glitching other way)
  int nl_count = 0;
  int rc_offset = 0;
  int pr_offset = 0;
  int recording = 0;
  char respCode[20];
  char pageResp[20];
  uint32_t rLeng=0;
  int16_t curY = 0;
  int16_t curX = 0;

  cls();
  unsigned long startTime = 0;

  lcd_out("starting wifi...");
  //cc3000.reboot();
  //cc3000.deleteProfiles();

  Adafruit_CC3000_Client www;

  lcd_outp(F("AP Connect"));
  showWifiCfg();

  if (!cc3000.connectToAP(cfg.ssid, cfg.pass, cfg.secMode)) goto CLEANUP;

  lcd_outp(F("DHCP"));
  for(ticks=0; ticks < MAX_TICKS; ticks++)
  {
    if( cc3000.checkDHCP() ) break;
    delay(100); 
  }  

  if(ticks >= MAX_TICKS) goto EXIT_FAIL;

  if(cfg.activeIP==0){
		if(cfg.debug_local==1) cfg.activeIP=ips2ip(cfg.test_ip);
		else{
			while  (cfg.activeIP  ==  0)  {
				if  (!  cc3000.getHostByName(cfg.server, &cfg.activeIP))  {
					if(WITH_SERIAL) Serial.println(F("Couldn't resolve!"));
				}
				delay(500);
			}  
		}
  }

  lcd_ip_out(cfg.activeIP);
  www = cc3000.connectTCP(cfg.activeIP, 80); //have been having occasional hang here...
  if ( !cc3000.checkConnected() ) goto EXIT_FAIL;

  lcd_outp(F("Connected"));
  
  if( www.connected() ) {
    tft.println("sending request");

	www.fastrprint(F("GET "));
	//tft.println("1");

	//breaking this up so no one sprintf takes up to much memory..
	www.fastrprint(WEBPAGE);
	//tft.println("1a");

	sprintf_P(buf, PSTR("?temp=%d&humi=%d&watered=%d&powerevt=%d"), (int)temp, (int)humi, watered, powerevt);
    www.fastrprint(buf);
	//tft.println("2");

	sprintf_P(buf, PSTR("&failure=%d&clientid=%d&smoked=%d&apikey="), failure, cfg.client_id, smoked);
    strcat(buf,cfg.apikey);
	www.fastrprint(buf);
	//tft.println("3");

    www.fastrprint(F(" HTTP/1.1\r\n"));
	//tft.println("4");

    www.fastrprint(F("Host: ")); www.fastrprint(cfg.server); www.fastrprint(F("\r\n"));
    www.fastrprint(F("\r\n"));
    if ( !cc3000.checkConnected() ) goto EXIT_FAIL;
    www.println();
  } 
  else{
    //lcd_out("Website Down?", 1);
    //delay(800); 
    goto EXIT_FAIL;
  }
  
  lcd_outp(F("Reading Response"));
  tft.print("Recv: ");
  startTime = millis();
  
  curY = tft.getCursorY();
  curX = tft.getCursorX();
  
   /* Read data until either the connection is closed, or the idle timeout is reached. */ 
  while (www.connected()) {
    
    if( timeDiff(startTime) > IDLE_TIMEOUT_MS ) break;

    while (www.available()) {

        if( timeDiff(startTime) > IDLE_TIMEOUT_MS ) break;
		
        char c = www.read();
		if(c != 0) rLeng++;
		
		if(WITH_SERIAL) Serial.print(c);

		if(rLeng % 25 == 0){ //(http headers have some length to them..)
			tft.fillRect(curX, curY, tft.width(), 30 ,ILI9341_BLACK);
			tft.setCursor(curX, curY);
			tft.print(rLeng);
		}

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
	 }
  }

  lcd_outp(F("\nClosing"));
  www.close();
  
  pageResp[pr_offset]=0;
  respCode[rc_offset]=0;
 
  if(rLeng <= 1){ 
	  lcd_outp(F("No response")); 
  }else{
	  if(rc_offset) lcd_out(respCode); else lcd_outp(F("Bad RespCode "));
	  if(pr_offset){
		  lcd_out(pageResp,1); 
		  uploads++;
		  if(uploads == 999) uploads = 1; //dont take up to much lcd space..
	  }
	  else lcd_outp(F("Bad PageResp "),1);
  }
  
  delay(2500);
  cc3000.disconnect(); /* must clean up or CC3000 can freak out next connect */
  //cc3000.stop();
  return true;
  
EXIT_FAIL:
   cc3000.disconnect(); 

   lcd_outp(F("Exit Fail?"), 1);
   delay(800); 

CLEANUP:
   //cc3000.stop();
   return false;
  
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

int dht22_read(uint8_t pin)
{
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
	humi = word(bits[0], bits[1]) *.1;//calculates and stores the humidity

	 uint8_t sign = 1;
     if (bits[2] & 0x80) // negative temperature
     {
            bits[2] = bits[2] & 0x7F;//negative temp adjustments
            sign = -1;
     }
    
	temp = toFahrenheit(sign * word(bits[2], bits[3]) * 0.1);//temp calculation and storage

	uint8_t sum = bits[0] + bits[1] + bits[2] + bits[3];//sum values

	if (bits[4] != sum) return -1;//failed checksum
	return 0;//great success!
}


// This function opens a Windows Bitmap (BMP) file and
// displays it at the given coordinates.  It's sped up
// by reading many pixels worth of data at a time
// (rather than pixel by pixel).  Increasing the buffer
// size takes more of the Arduino's precious RAM but
// makes loading a little faster.  20 pixels seems a
// good balance.

#define BUFFPIXEL 20

void bmpDraw(char *filename, uint8_t x, uint16_t y) {

	File     bmpFile;
	int      bmpWidth, bmpHeight;   // W+H in pixels
	uint8_t  bmpDepth;              // Bit depth (currently must be 24)
	uint32_t bmpImageoffset;        // Start of image data in file
	uint32_t rowSize;               // Not always = bmpWidth; may have padding
	uint8_t  sdbuffer[3*BUFFPIXEL]; // pixel buffer (R+G+B per pixel)
	uint8_t  buffidx = sizeof(sdbuffer); // Current position in sdbuffer
	boolean  flip    = true;        // BMP is stored bottom-to-top
	int      w, h, row, col;
	uint8_t  r, g, b;
	uint32_t pos = 0, startTime = millis();

	bool debugDraw = true;
	if(WITH_SERIAL==0) debugDraw = false;

	if( (x >= tft.width()) || (y >= tft.height()) ) return;

	if(WITH_SERIAL) Serial.print("Loading image ");
	if(WITH_SERIAL) Serial.println(filename);

	// Open requested file on SD card
	bmpFile = SD.open(filename);
	if(bmpFile == NULL) {
		if(WITH_SERIAL) Serial.print("File not found");
		return;
	}

	// Parse BMP header
	uint16_t signature = read16(bmpFile);// BMP signature
	uint32_t fsize = read32(bmpFile);
	read32(bmpFile);                  // Read & ignore creator bytes
	bmpImageoffset = read32(bmpFile); // Start of image data
	uint32_t hSize = read32(bmpFile); // Read DIB header
	bmpWidth  = read32(bmpFile);
	bmpHeight = read32(bmpFile);
    uint16_t planes = read16(bmpFile);
	bmpDepth = read16(bmpFile); // bits per pixel
	uint32_t compressed = read32(bmpFile);
	uint8_t errorCode = 0;

	if(signature != 0x4D42)                  { errorCode=1; goto CLEANUP;}
	if(planes != 1)                          { errorCode=2; goto CLEANUP;} // # planes -- must be '1'
	if((bmpDepth != 24) && (compressed != 0)){ errorCode=3; goto CLEANUP;} // 0 = uncompressed

	if(debugDraw){ 
		Serial.print(F("File size: ")); Serial.println(fsize); 
		Serial.print(F("Image Offset: ")); Serial.println(bmpImageoffset, DEC);
		Serial.print(F("Header size: ")); Serial.println(hSize);
		Serial.print(F("Bit Depth: ")); Serial.println(bmpDepth);
		Serial.print(F("Image size: "));
		Serial.print(bmpWidth);
		Serial.print('x');
		Serial.println(bmpHeight);
	}

	// BMP rows are padded (if needed) to 4-byte boundary
	rowSize = (bmpWidth * 3 + 3) & ~3;

	// If bmpHeight is negative, image is in top-down order.
	// This is not canon but has been observed in the wild.
	if(bmpHeight < 0) {
		  bmpHeight = -bmpHeight;
		  flip      = false;
	}

	// Crop area to be loaded
	w = bmpWidth;
	h = bmpHeight;
	if((x+w-1) >= tft.width())  w = tft.width()  - x;
	if((y+h-1) >= tft.height()) h = tft.height() - y;

	// Set TFT address window to clipped image bounds
	tft.setAddrWindow(x, y, x+w-1, y+h-1);

	for (row=0; row<h; row++) { // For each scanline...
		  // Seek to start of scan line.  It might seem labor-
		  // intensive to be doing this on every line, but this
		  // method covers a lot of gritty details like cropping
		  // and scanline padding.  Also, the seek only takes
		  // place if the file position actually needs to change
		  // (avoids a lot of cluster math in SD library).

		  if(flip) // Bitmap is stored bottom-to-top order (normal BMP)
				pos = bmpImageoffset + (bmpHeight - 1 - row) * rowSize;
		  else     // Bitmap is stored top-to-bottom
				pos = bmpImageoffset + row * rowSize;

		  if(bmpFile.position() != pos) { // Need seek?
				bmpFile.seek(pos);
				buffidx = sizeof(sdbuffer); // Force buffer reload
		  }

		  for (col=0; col<w; col++) { // For each pixel...
				// Time to read more pixel data?
				if (buffidx >= sizeof(sdbuffer)) { // Indeed
					  bmpFile.read(sdbuffer, sizeof(sdbuffer));
					  buffidx = 0; // Set index to beginning
				}

				// Convert pixel from BMP to TFT format, push to display
				b = sdbuffer[buffidx++];
				g = sdbuffer[buffidx++];
				r = sdbuffer[buffidx++];
				tft.pushColor(tft.color565(r,g,b));
		  } // end pixel

	} // end scanline

	if(debugDraw){
		Serial.print(F("Loaded in "));
		Serial.print(millis() - startTime);
		Serial.println(" ms");
	}
   
 
CLEANUP:
  bmpFile.close();

  if(WITH_SERIAL && errorCode > 0){
	  switch(errorCode){
		  case 1: Serial.println(F("Bad BMP signature")); break;
		  case 2: Serial.println(F("Planes must be 1")); break;
		  case 3: Serial.println(F("Must be 24bit and uncompressed.")); break;
		  default: Serial.println(F("BMP format not recognized."));
	  }
  }

}

// These read 16- and 32-bit types from the SD card file.
// BMP data is stored little-endian, Arduino is little-endian too.
// May need to reverse subscript order if porting elsewhere.

uint16_t read16(File &f) {
  uint16_t result;
  ((uint8_t *)&result)[0] = f.read(); // LSB
  ((uint8_t *)&result)[1] = f.read(); // MSB
  return result;
}

uint32_t read32(File &f) {
  uint32_t result;
  ((uint8_t *)&result)[0] = f.read(); // LSB
  ((uint8_t *)&result)[1] = f.read();
  ((uint8_t *)&result)[2] = f.read();
  ((uint8_t *)&result)[3] = f.read(); // MSB
  return result;
}

static int myini_handler(void* user, const char* section, const char* name, const char* value)
{
	//we can ignore the section since all names are unique..
	//should we do a strcpy to local buffer instead? they are never freed so no holes but allocs can be shady..should be ok..
	if (strcmp(name, ssid)    ==0) cfg.ssid = strdup(value);
	if (strcmp(name, security)==0) cfg.security = strdup(value);
	if (strcmp(name, pass)    ==0) cfg.pass = strdup(value);
	if (strcmp(name, apikey)  ==0) cfg.apikey = strdup(value);
	if (strcmp(name, server)  ==0) cfg.server = strdup(value);
	if (strcmp(name, test_ip) ==0) cfg.test_ip = strdup(value);

	if (strcmp(name, ext_watchdog) ==0) cfg.ext_watchdog = atoi(value);
	if (strcmp(name, temp_shift)   ==0) cfg.temp_shift = atoi(value);
	if (strcmp(name, humi_shift)   ==0) cfg.humi_shift = atoi(value);
	if (strcmp(name, debug_local)  ==0) cfg.debug_local = atoi(value);
	if (strcmp(name, client_id)    ==0) cfg.client_id = atoi(value);
	if (strcmp(name, demoMode)     ==0) cfg.demoMode = atoi(value);
	if (strcmp(name, speedMode)    ==0) cfg.speedMode = atoi(value);
    
//	tft.println(name);

    return 1;
}


char* my_fgets(char* buf, int len, File* stream){

	if(stream->available()){
		int i = stream->readBytesUntil('\n',buf,len);
		if(buf[i-1] == '\r') buf[i-1] = 0;
		return buf;
	}
	else
		return 0;
	
}

bool loadConfig(){

	tft.println("Loading config");

	memset(&cfg,0, sizeof(cfg));

	bool errorDisplayed = false;

	lcd_out("Init SD card...");

	while(!SD.begin(SD_CS)) {
		if(!errorDisplayed){
			tft.println("Missing...");
			tft.println("Insert SD Card.");
			errorDisplayed = true;
		}
		delay(1500);
	}

	if(!SD.exists(iniFile)){
		tft.println("Stopped, missing");
		tft.println(iniFile);
		while(1);
	}

	File file = SD.open(iniFile);
	if(!file) while(1);

	if(ini_parse_stream((ini_reader)my_fgets, &file, myini_handler, 0) < 0) {
		tft.println("IniParse Failed..");
		tft.println("Stopped");
		while(1);
	}
	
	file.close();

	int fail=0;
	if(cfg.ssid==0){ps(missing, ssid);fail++;}
	if(cfg.security==0){ps(missing, security);fail++;}
	if(cfg.pass==0){ps(missing, pass);fail++;}
	if(cfg.client_id==0){ps(missing, client_id);fail++;}
	if(cfg.apikey==0){ps(missing, apikey);fail++;}
	if(cfg.server==0){ps(missing, server);fail++;}

	if(cfg.debug_local!=0){
		if(cfg.test_ip==0){ps(missing, test_ip);fail++;}
	}
  
	if(strcmp(cfg.security,"wep")==0){
		HexToBin(cfg.pass);
		cfg.secMode=WLAN_SEC_WEP;
	}

  if(strcmp(cfg.security,"none")==0) cfg.secMode=WLAN_SEC_UNSEC;
  if(strcmp(cfg.security,"wpa")==0)  cfg.secMode=WLAN_SEC_WPA;
  if(strcmp(cfg.security,"wpa2")==0) cfg.secMode=WLAN_SEC_WPA2;
  
  if(fail!=0){tft.print("Ini Errors: "); tft.println(fail);}

  cls();
  showCfg(false);
  delay(1500);

  return true;

}

void hexDump(char* input){

	char tmp[5];
	uint8_t *p = (uint8_t*)input;
	for(int i=0;i<strlen(input);i++){
		sprintf(tmp, "%02x ", p[i]);
		tft.print(tmp);
	}

	tft.println();

}

int HexToBin(char* input){

	char buf[30];
	char *h = input;
	unsigned char *b = (unsigned char*)buf; /* point inside the buffer */
    int cnt=0;
	int sz=strlen(input);

	/* offset into this string is the numeric value */
	char xlate[] = "0123456789abcdef";

	//printf("translating..\n");
	for ( ; *h; h += 2){ /* go by twos through the hex string multiply leading digit by 16 */
	    if(isupper(*h)) *h = tolower(*h);
		if(isupper(*(h+1))) *(h+1) = tolower(*(h+1));
		*b = ((strchr(xlate, *h) - xlate) * 16) + ((strchr(xlate, *(h+1)) - xlate));
	    b++;
		cnt++;
	}

	memset(input,0,sz);
	strncpy(input,buf,cnt);
	return cnt;
		
}

void showWifiCfg(){
    ps("ssid: ", cfg.ssid);
	ps("sec: ", cfg.security);

	if(strcmp(cfg.security,"wep")==0){
		tft.print("pass:"); 
		if(cfg.demoMode) tft.println(stars);
		else hexDump(cfg.pass);
	}else{
		tft.print("pass: "); 
		if(cfg.demoMode) tft.println(stars);
		else tft.println(cfg.pass);
	}
}

void configBanner(int tab){
	tft.setTextColor( (tab==0 ? ILI9341_WHITE : ILI9341_DARKGREY) );
	tft.print("Show |");

	tft.setTextColor( (tab==1 ? ILI9341_WHITE : ILI9341_DARKGREY));
	tft.print("  Modify  |");

	tft.setTextColor(ILI9341_RED);
	tft.println("  X");

	tft.setTextColor(ILI9341_WHITE);	
}

void showCfg(bool block){

	cls();
	if(block) configBanner(curTab); else curTab=0;

	if(curTab==0){
		tft.println("       [wifi]");
		showWifiCfg();

		tft.println("      [config]");
		ps("dog: ", cfg.ext_watchdog);
		ps("tshift: ", cfg.temp_shift);
		ps("hshift: ", cfg.humi_shift);
		ps("debug: ", cfg.debug_local);
		ps("demo: ", cfg.demoMode);
		ps("speed: ", cfg.speedMode);

		tft.println("       [web]");
		ps("uid: ", cfg.client_id);

		tft.print("key: ");
		if(cfg.demoMode) tft.println(stars);
		else tft.println(cfg.apikey);
		
		ps("srv: ", cfg.server);
		ps("tstIp: ", cfg.test_ip);
	}
	else{
		tft.setTextSize(3);
		tft.print("local = "); tft.println(cfg.debug_local); tft.println();
		tft.print("demo  = "); tft.println(cfg.demoMode);    tft.println();
		tft.print("speed = "); tft.println(cfg.speedMode);   tft.println();
		tft.setTextSize(2);
	}

	if(block){
		while(1){
			//y = > 300 in top menu bar..
			//x: < 30 = exit, 36-160 = modify, 180+ = show
			if ( ctp.touched() ) {
			    TS_Point p = ctp.getPoint(); 
				//sprintf(tmp,"\nx:%d\ny:%d", p.x, p.y);
				//tft.println(tmp);
				delay(500); //software debounce

				if(p.y > 300){ //top tab bar..
					if(p.x < 30) break; //X pressed

					if(p.x > 180){ //show tab
						curTab = 0;
						showCfg(true);
						break;
					}else{ //modify tab
						curTab = 1;
						showCfg(true);
						break;
					}

				}	
				
				if(curTab==1){
					if(p.y > 250){
						cfg.debug_local = cfg.debug_local == 0 ? 1 : 0;
						cfg.activeIP = 0; //force lookup next cycle (ip gets cached)
						showCfg(true);
						break;
					}else if(p.y <= 250 && p.y >= 210){
						cfg.demoMode = cfg.demoMode == 0 ? 1 : 0;
						showCfg(true);
						break;
					}else if(p.y <= 200 && p.y >= 170){
						cfg.speedMode = cfg.speedMode == 0 ? 1 : 0;
						showCfg(true);
						break;
					}

				}

			}
		}
	}

	if(!block){
		//hidden option, if they touch the screen during config display 
		//they can enter config mode (useful at startup)
		for(int i=0; i <= 10; i++){
			delay(250);
			if( ctp.touched() ){
				showCfg(true);
				return;
			}
		}
	}

}

void ps(char* name, char* value){ tft.print(name); tft.println(value);}
void ps(char* name, uint16_t value){ tft.print(name); tft.println(value);}

//ip string to ipu32 -  usage:  uint32_t ip = ips2ip("192.168.0.10");
uint32_t ips2ip(char* ips){
  int i=0, j=0;
  uint8_t b[4]={0,0,0,0};
  char tmp[5]={0,0,0,0,0};

  while(*ips){
      if(*ips=='.'){
          b[j++] = atoi(tmp);
          for(; i>=0; i--) tmp[i]=0; //wipe tmp for next
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