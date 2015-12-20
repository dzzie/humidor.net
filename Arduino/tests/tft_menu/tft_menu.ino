#include <SPI.h>
#include <Wire.h>
#include <string.h>
#include <stdio.h>
#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ILI9341.h>
#include <Adafruit_FT6206.h>
#include <SPI.h>


// The FT6206 uses hardware I2C (SCL/SDA)
Adafruit_FT6206 ctp = Adafruit_FT6206();

#define TFT_CS 10
#define TFT_DC 9
#define SD_CS 4
Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC);

char tmp[100];

int curTab = 0;

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

CONFIG cfg;

void cls(){
	tft.fillScreen(ILI9341_BLACK);
    tft.setCursor(0,0);
}

void loop(void)
{
  
}


void setup(void)
{

	Serial.begin(9600); 
	while(!Serial);
	Serial.println("Init tft..");

	tft.begin();
	tft.setTextSize(2);
    
	cls();

	if ( !ctp.begin(40) ) {  // pass in 'sensitivity' coefficient
		tft.print("Failed..");
		while (1);
	}


	showCfg(true);
	

	 


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
		tft.println("[wifi]");
		ps("ssid: ", "");
		ps("sec: ", ""); 
		ps("pass: ", "");
		tft.println("[config]");
		/*ps("dog: ", "");
		ps("tshift: ", "");
		ps("hshift: ", "");
		ps("debug: ", "");
		tft.println("[web]");
		ps("uid: ", "");
		ps("key: ", "");
		ps("srv: ", "");
		ps("testIp: ", "");*/
	}else{
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
				sprintf(tmp,"\nx:%d\ny:%d", p.x, p.y);
				tft.println(tmp);
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
					if(p.y > 300 && p.x < 30) break; //X pressed

					if(p.y > 250){
						cfg.debug_local = cfg.debug_local == 0 ? 1 : 0;
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

	cls();

}

void ps(char* name, char* value){ tft.print(name); tft.println(value);}
void ps(char* name, uint16_t value){ tft.print(name); tft.println(value);}
