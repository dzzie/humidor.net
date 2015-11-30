
/*before fast string conv:

    Binary sketch size: 27,166 bytes (used 84% of a 32,256 byte maximum) (1.14 secs)
    Minimum Memory Usage: 1521 bytes (74% of a 2048 byte maximum)

 now:
    Binary sketch size: 27,252 bytes (used 84% of a 32,256 byte maximum) (0.91 secs)
    Minimum Memory Usage: 1127 bytes (55% of a 2048 byte maximum)
*/

#include <Bridge.h>
#include <HttpClient.h>
#include <SPI.h>
#include <Wire.h>
#include <string.h>
#include <stdio.h>
#include <avr/wdt.h>
#include <Bridge.h>
#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7735.h> // Hardware-specific library 
#include <SPI.h>
#include <FileIO.h>

#define TFT_DC      8
#define TFT_RST     9 
#define TFT_CS     10
#define TFT_MOSI   11 
#define TFT_SCLK   12  

#define WITH_SERIAL 1

/* sample config.txt file 

6,0,2,test,sandsprite.com

# first line must be the config settings
# the format is userid, deltaT, deltaH, apikey, server  
# no spaces, this data after doent matter.

*/


/*
Copyright David Zimmer <dzzie@yahoo.com>
WebSite:  http://sandsprite.com
All rights reserved, no portion of this code is authorized for sale or redistribution
*/

//use test server, no dht22 required
#define debug_local   1    
#define dht22_pin     2
#define EXT_WATCHDOG_PIN 6

#define WEBPAGE       "/humidor/logData.php" 
#define firmware_ver  "v1.3 " __DATE__  
#define MAX_TICKS 1000

char *DFAIL = "DHT22 Fail";

volatile int counter;      // Count number of times ISR is called.
volatile int countmax = 8; // Timer expires after about 64 secs (using 8 sec interval)

int speedMode = 1;
uint8_t powerevt  = 1;
uint8_t watered   = 0;
uint8_t smoked    = 0;
uint8_t failure   = 0;
uint8_t inReadSensor  = 0;
int uploads  = 0;
int fail_cnt = 0;

uint8_t uid = 0;
char apikey[20]={0};
char hostname[30]={0};
char localIP[25] = {0};
char tmp[55] = {0};

uint8_t delta_t = 0;
uint8_t delta_h = 0;
double temp      = 0;
double humi      = 0;
double last_temp = 0;
double last_humi = 0;

Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_MOSI, TFT_SCLK, TFT_RST);

unsigned long IDLE_TIMEOUT_MS = 7000; // Amount of time to wait (in milliseconds) with no data (7 seconds)
                                      // received before closing the connection.  If you know the server
                                      // you're accessing is quick to respond, you can reduce this value. 



//following will allow you to add conditional compilation of serial debugging code 
//with single line statements. saves space when disabled..
#ifdef WITH_SERIAL
   #define IFS(x) x
#else
   #define IFS(x)
#endif

void cls(){
	tft.fillScreen(ST7735_BLACK);
    tft.setCursor(0,0);
}

void setup(void)
{
  
  Serial.begin(9600); 
  tft.initR(INITR_BLACKTAB);   
  cls();
  tft.println("Starting Bridge...");
  //todo display graphic for long delay..

  pinMode(13, OUTPUT);
  digitalWrite(13, LOW);
  Bridge.begin();
  digitalWrite(13, HIGH);

  FileSystem.begin();

  IFS(Serial.begin(9600);)
  //IFS(while(!Serial);)

  lcd_outp(F("Daves WebHumidor"));
  lcd_out(firmware_ver,1);

}

void loop(void)
{
   
   if(!readConfig()) return;

   if(debug_local){
       temp = 66; humi = 66;
   }else{
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

   show_readings();
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
   }

   watchdogDisable();
   
   show_readings();
   
   if(fail_cnt > 2){
		sprintf_P(tmp, PSTR("%d Fails"), fail_cnt);
   }
   else{//track # successful uploads since last reset (watch watchdog)
		sprintf(tmp, "%d", uploads);
   }

	//lcd.setCursor(15-strlen(tmp),1); 
	//lcd.print(tmp);
	delay_x_min(30); //webui expects 30min delay for stat gen
}

void watchdogReset()
{
#if USE_EXT_WATCHDOG
	pinMode(EXT_WATCHDOG_PIN, OUTPUT);
	delay(200);
	pinMode(EXT_WATCHDOG_PIN, INPUT);
#else
	wdt_reset();
#endif
}

void watchdogDisable()
{
#if USE_EXT_WATCHDOG
	pinMode(EXT_WATCHDOG_PIN, OUTPUT);
#else
	wdt_disable();
#endif
}

//updated code to extend watch dog to 24 seconds.
//special thanks to Philip Allagas for bringing this post to my attention
//and Dave Evans for the code :)
//http://forum.arduino.cc/index.php?topic=248263.0 

void watchdogEnable()
{
#if USE_EXT_WATCHDOG
	watchdogReset();
#else
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
#endif

}

#if USE_EXT_WATCHDOG == 0
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
#endif

void show_readings(){
     
	  uint16_t hColor = ST7735_GREEN;
	  uint16_t tColor = ST7735_GREEN;

	  if((int)temp < 60 || (int)temp > 75) tColor = ST7735_RED;
	  if((int)humi < 60 || (int)humi > 75) hColor = ST7735_RED;

	  cls();
  
	  tft.setTextSize(2);
	  tft.print("Temp:");

	  tft.setTextColor(tColor);
	  tft.setTextSize(5);
	  tft.print((int)temp);
	  
	  tft.setTextSize(1);
	  tft.println();
	  
	  tft.setTextColor(ST7735_WHITE);
	  tft.setTextSize(2);
	  tft.print("Humi:");

	  tft.setTextColor(hColor);
	  tft.setTextSize(5);
	  tft.print((int)humi);

	  tft.setTextColor(ST7735_WHITE);
	  tft.setTextSize(1);

	  IFS(Serial.print("X:");Serial.println(tft.getCursorX());)
}

void lcd_out(char* s){ lcd_out(s,0); }
  
void lcd_out(char* s, int row){
    //if(row==0) //lcd.clear();
    //lcd.setCursor(0,row); 
    //lcd.print(s);
	/*tft.setTextSize(1);
	if(tft.getCursorX() >= tft.height()){
		IFS(Serial.print("_X:");Serial.println(tft.getCursorX());)
		tft.setCursor(90,0);
		for(int i=0; i<10;i++) tft.println();
		tft.setCursor(90,0);
	}*/
	tft.println(s);
}

void lcd_outp(const __FlashStringHelper *s){lcd_outp(s,0); }

void lcd_outp(const __FlashStringHelper *s, int row){
    uint8_t c;
	const char PROGMEM *p = (const char PROGMEM *)s;
    while ((c = pgm_read_byte_near(p++)) != 0) tft.print((char)c);
}

void delay_x_min(int minutes){
    delay_x_min(minutes, 0);
}

void displayFlags(int min){
	tft.setTextSize(2);
	sprintf_P(tmp, PSTR("%d min "), min); //fixed display bug lcd not overwriting tens digit once <= 9
	tft.fillRect(0, tft.width() - 5, tft.width(), 20, ST7735_BLACK);
	tft.setCursor(0, tft.width() - 5);
	tft.print(tmp);
	if(watered) tft.print("W "); 
    if(smoked)  tft.print('S'); 
	tft.setTextSize(1);
}

void delay_x_min(int minutes, int silent){

  for(int i=0; i < minutes; i++){
      
      //re-read the sensor every minute to update display? thats allot of sensor reads.. whats its lifetime?
      
      if(silent==0) displayFlags( minutes - i );

      for(int j=0; j < 240; j++){ //entire j loop = one minute, using small delay so buttons responsive.. 
		  delay( (speedMode==1 ? 1 : 250) );  
          //uint8_t buttons = lcd.readButtons();
          /*if (buttons && (buttons & BUTTON_SELECT) ){
              watered = 1;        
              displayFlags();
          } 
		  if (buttons && (buttons & BUTTON_DOWN) ){
              smoked = 1;        
              displayFlags();
          }  
		  if (buttons && (buttons & BUTTON_UP) ){
			  if( ReadSensor() ) show_readings(); else lcd_outp(F("Read Fail?"));
		  }
		  #if debug_local
				  if (buttons && (buttons & BUTTON_RIGHT) ){ //speed up clock to test 6x pump delay..
			//		  lcd.setCursor(14,1); 
					  if(speedMode==1){
						  speedMode = 0; 
			//			  lcd.print(' ');
					  }else{
						  speedMode = 1;
			//			  lcd.print('F');
					  }     
					  delay(500); //if you are toggling a field, you NEED the delay..
				  }
		  #endif
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

  humi += delta_h;
  temp += delta_t;

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

bool PostData()
{
  char buf[150];
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

  unsigned long startTime = 0;
  
  cls();
  lcd_outp(F("Submit "));

  strcpy(buf,"http://");
  strcat(buf,hostname);

  //breaking this up so no one sprintf takes up to much memory..
  strcat_P(buf, PSTR(WEBPAGE));
  sprintf_P(tmp, PSTR("?temp=%d&humi=%d&watered=%d&powerevt=%d"), (int)temp, (int)humi, watered, powerevt);
  strcat(buf,tmp);

  sprintf_P(tmp, PSTR("&failure=%d&clientid=%d&smoked=%d&apikey="), failure, uid, smoked);
  strcat(buf,tmp);
  strcat(buf,apikey);
  
  HttpClient client;
  client.setTimeout(7000);
  client.get(buf);

  sprintf_P(tmp, PSTR("%d bytes "), strlen(buf) );
  lcd_out(tmp,1);

  delay(1200);
  
  lcd_outp(F("Reading Response"));
  //lcd_outp(F("Recv: "),1);
  startTime = millis();

   /* Read data until either the connection is closed, or the idle timeout is reached. */ 
    while( client.available() ) {

        //if( timeDiff(startTime) > IDLE_TIMEOUT_MS ) break;
		
        char c = client.read();
		if(c != 0) rLeng++;

		/*if(rLeng % 25 == 0){ //(http headers have some length to them..)
			//lcd.setCursor(13,1); 
			//lcd.print(rLeng);
		}*/

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
  }

  lcd_outp(F("Closing"));
  
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
  return true;
  
EXIT_FAIL:

   lcd_outp(F("Exit Fail?"), 1);
   delay(800); 

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


//this config mechanism requies 10% memory..
bool readConfig()
{
  bool configOk = false;
  
  if(uid!=0) return true;

  tft.println("Reading Config...");
  delay(5000);
  File dataFile = FileSystem.open("/mnt/sd/config.txt", FILE_READ);
  if (dataFile) {
	  size_t sz = dataFile.readBytesUntil(',', &tmp[0],32);
	  if(sz > 0){
			uid = atoi(tmp) & 0x000000FF;
			tft.print("Uid: ");
			tft.println(uid);
			sz = dataFile.readBytesUntil(',', &tmp[0],32);
			if(sz > 0){
				delta_t = atoi(tmp) & 0x000000FF;
				tft.print("DeltaT: ");
				tft.println(delta_t);
				sz = dataFile.readBytesUntil(',', &tmp[0],32);
				if(sz > 0){
					delta_h = atoi(tmp) & 0x000000FF;
					tft.print("DeltaH: ");
					tft.println(delta_h);
					sz = dataFile.readBytesUntil(',', &tmp[0],32);
					if(sz > 0){
						strcpy(apikey,tmp);
						tft.print("ApiKey: ");
						tft.println(apikey);
						sz = dataFile.readBytesUntil(',', &tmp[0],32);
						if(sz > 0){
							strcpy(hostname,tmp);
							tft.print("Host: ");
							tft.println(hostname);
							configOk = true;
						}
					}
				}
			}
	  }
	  if(uid==0) configOk = false;
	  dataFile.close();
  }else{
	  tft.println("cant open config.txt?");
	  delay(1000);
  } 
  if(!configOk){
	  tft.println("Invalid config.");
	  delay(1000);
  }
  delay(3000);
  return configOk;
}



