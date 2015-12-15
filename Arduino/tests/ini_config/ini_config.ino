#include <SPI.h>
#include <Wire.h>
#include <string.h>
#include <stdio.h>
#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ILI9341.h>
#include <SPI.h>
//#include <FileIO.h>
#include <SD.h>
#include "IniFile.h"

#define TFT_CS 10
#define TFT_DC 9
#define SD_CS 4
Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC);

struct CONFIG{
	uint16_t temp_shift;   
	uint16_t humi_shift;
	uint16_t client_id;
	uint16_t ext_watchdog;
	uint16_t debug_local;
	char apikey[30];
	char server[30];
	char test_ip[30];
	char WLAN_SSID[30]; // cannot be longer than 32 characters!
	char WLAN_SECURITY[30]; // Security can be WLAN_SEC_UNSEC, WLAN_SEC_WEP, WLAN_SEC_WPA or WLAN_SEC_WPA2
	char WLAN_PASS[30];
	uint32_t activeIP; //filled out at runtime from string either dns lookup or from ip string..
};

CONFIG cfg;
char tmp[100];

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

  tft.println("Init SD card...");
  if (!SD.begin(SD_CS)) {
		tft.println("Failed!");
  }

  IniFile ini("public.ini");

  /*
	[wifi]
	ssid         = mywifiname
	security     = wep
	pass         = baadfood

	[config]
	ext_watchdog = 1
	temp_shift   = 0
	humi_shift   = 2
	debug_local  = 0

	[web]
	client_id    = 1
	apikey       = password
	server       = sandsprite.com
	test_ip      = 192.168.0.10
  
	struct CONFIG{
		uint16_t temp_shift;   
		uint16_t humi_shift;
		uint16_t client_id;
		uint16_t ext_watchdog;
		uint16_t debug_local;
		char apikey[30];
		char server[30];
		char test_ip[30];
		char WLAN_SSID[30]; // cannot be longer than 32 characters!
		char WLAN_SECURITY[30]; // Security can be WLAN_SEC_UNSEC, WLAN_SEC_WEP, WLAN_SEC_WPA or WLAN_SEC_WPA2
		char WLAN_PASS[30];
	}
  */

  memset(&cfg,0, sizeof(cfg));
  
  if (!ini.open()) {
    tft.println("No Ini file");
    tft.print(ini.getFilename());
    while(1);
  }

  int fail=0;

  if( !ini.getValue("wifi","ssid" , cfg.WLAN_SSID, 30) ){
	  ps("ssid: ", ini.s_getError());fail++;
  }

  if( !ini.getValue("wifi", "security", cfg.WLAN_SECURITY, 30) ){
	  ps("security: ", ini.s_getError());fail++;
  }

  if( !ini.getValue("wifi", "pass", cfg.WLAN_PASS, 30) ){
	  ps("pass: ", ini.s_getError());fail++;
  }
 
  if( !ini.getValue("config", "ext_watchdog", tmp, sizeof(tmp), cfg.ext_watchdog) ){
	  ps("ext_watchdog: ", ini.s_getError());fail++;
  }

  if( !ini.getValue("config", "temp_shift", tmp, sizeof(tmp), cfg.temp_shift) ){
	  ps("temp_shift: ", ini.s_getError());fail++;
  }

  if( !ini.getValue("config", "humi_shift", tmp, sizeof(tmp), cfg.humi_shift) ){
	  ps("humi_shift: ", ini.s_getError());fail++;
  }

  if( !ini.getValue("config", "debug_local", tmp, sizeof(tmp), cfg.debug_local) ){
	  ps("debug_local: ", ini.s_getError());fail++;
  }

  if( !ini.getValue("web", "client_id", tmp, sizeof(tmp), cfg.client_id) ){
	  ps("client_id: ", ini.s_getError());fail++;
  }

  if( !ini.getValue("web", "apikey", cfg.apikey, 30) ){
	  ps("apikey: ", ini.s_getError());fail++;
  }

  if( !ini.getValue("web", "server", cfg.server, 30) ){
	  ps("server: ", ini.s_getError());fail++;
  }

  if( !ini.getValue("web", "test_ip", cfg.test_ip, 30) ){
	  ps("test_ip: ", ini.s_getError());fail++;
  }

  ini.close();

  if(strcmp(cfg.WLAN_SECURITY,"wep")==0){
		HexToBin(cfg.WLAN_PASS);
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
	ps("ssid: ", cfg.WLAN_SSID);
	ps("sec: ", cfg.WLAN_SECURITY);

	if(strcmp(cfg.WLAN_SECURITY,"wep")==0){
		tft.print("pass: "); hexDump(cfg.WLAN_PASS);
	}else{
		ps("pass: ", cfg.WLAN_PASS);
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
