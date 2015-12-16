#include <SPI.h>
#include <Wire.h>
#include <string.h>
#include <stdio.h>
#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ILI9341.h>
#include <SPI.h>
#include <SD.h>
#include "ini.h"

#define TFT_CS 10
#define TFT_DC 9
#define SD_CS 4
Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC);

struct CONFIG{
	int temp_shift;   
	int humi_shift;
	int client_id;
	int ext_watchdog;
	int debug_local;
	int demoMode;
	char* apikey;
	char* server;
	char* test_ip;
	char* ssid;     // cannot be longer than 32 characters!
	char* security; // Security can be WLAN_SEC_UNSEC, WLAN_SEC_WEP, WLAN_SEC_WPA or WLAN_SEC_WPA2
	char* pass;
	uint32_t activeIP;   //filled out at runtime from string either dns lookup or from ip string..
	uint8_t secMode;
};

struct CFG_STRINGS{
	char* ssid = "ssid";
	char* security = "security";
	char* pass = "pass";
	char* ext_watchdog = "ext_watchdog";
	char* temp_shift = "temp_shift";
	char* humi_shift = "humi_shift";
	char* debug_local = "debug_local";
	char* demoMode = "demoMode";
	char* client_id = "client_id";
	char* apikey = "apikey";
	char* server = "server";
	char* test_ip = "test_ip";
	char* missing = "Missing ";
};

CONFIG cfg;
CFG_STRINGS sCfg;

char tmp[100];

void cls(){
	tft.fillScreen(ILI9341_BLACK);
    tft.setCursor(0,0);
}

void loop(void)
{
  
}

static int myini_handler(void* user, const char* section, const char* name, const char* value)
{
   
	//we can ignore the section since all names are unique..
    
	if (strcmp(name, sCfg.ssid)    ==0) cfg.ssid = strdup(value);
	if (strcmp(name, sCfg.security)==0) cfg.security = strdup(value);
	if (strcmp(name, sCfg.pass)    ==0) cfg.pass = strdup(value);
	if (strcmp(name, sCfg.apikey)  ==0) cfg.apikey = strdup(value);
	if (strcmp(name, sCfg.server)  ==0) cfg.server = strdup(value);
	if (strcmp(name, sCfg.test_ip) ==0) cfg.test_ip = strdup(value);

	if (strcmp(name, sCfg.ext_watchdog) ==0) cfg.ext_watchdog = atoi(value);
	if (strcmp(name, sCfg.temp_shift)   ==0) cfg.temp_shift = atoi(value);
	if (strcmp(name, sCfg.humi_shift)   ==0) cfg.humi_shift = atoi(value);
	if (strcmp(name, sCfg.debug_local)  ==0) cfg.debug_local = atoi(value);
	if (strcmp(name, sCfg.client_id)    ==0) cfg.client_id = atoi(value);
    
//	tft.println(name);


    return 1;
}

void setup(void)
{

	Serial.begin(9600); 
	while(!Serial);
	Serial.println("Init tft..");

	tft.begin();
	tft.setTextSize(2);

	cls();

	tft.println("Init SD card...");
	if (!SD.begin(SD_CS)) {
		tft.println("Failed!");
	}

	memset(&cfg,0, sizeof(cfg));

	if (ini_parse("private.ini", myini_handler, 0) < 0) {
	   tft.println("No Ini file");
	   while(1);
	}

	int fail=0;
	if(cfg.ssid==0){ps(sCfg.missing, sCfg.ssid);fail++;}
	if(cfg.security==0){ps(sCfg.missing, sCfg.security);fail++;}
	if(cfg.pass==0){ps(sCfg.missing, sCfg.pass);fail++;}
	if(cfg.client_id==0){ps(sCfg.missing, sCfg.client_id);fail++;}
	if(cfg.apikey==0){ps(sCfg.missing, sCfg.apikey);fail++;}
	if(cfg.server==0){ps(sCfg.missing, sCfg.server);fail++;}

	if(cfg.debug_local!=0){
		if(cfg.test_ip==0){ps(sCfg.missing, sCfg.test_ip);fail++;}
	}
  
	if(strcmp(cfg.security,"wep")==0){
		HexToBin(cfg.pass);
	}

	if(fail!=0){tft.print("Ini Errors: "); tft.println(fail);}
	showCfg();




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

void showCfg(){
	tft.println("[wifi]");
	ps("ssid: ", cfg.ssid);
	ps("sec: ", cfg.security);

	if(strcmp(cfg.security,"wep")==0){
		tft.print("pass: "); hexDump(cfg.pass);
	}else{
		ps("pass: ", cfg.pass);
	}

	tft.println("[config]");
	ps("dog: ", cfg.ext_watchdog);
	ps("tshift: ", cfg.temp_shift);
	ps("hshift: ", cfg.humi_shift);
	ps("debug: ", cfg.debug_local);

	tft.println("[web]");
	ps("uid: ", cfg.client_id);
	ps("key: ", cfg.apikey);
	ps("srv: ", cfg.server);
	ps("testIp: ", cfg.test_ip);
}

void ps(char* name, char* value){ tft.print(name); tft.println(value);}
void ps(char* name, uint16_t value){ tft.print(name); tft.println(value);}
