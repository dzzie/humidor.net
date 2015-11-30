
/*
	status: complete

	Binary sketch size: 5,982 bytes (used 19% of a 32,256 byte maximum) (0.50 secs)
	Minimum Memory Usage: 475 bytes (23% of a 2048 byte maximum)

	This sketch will
		* read config struct from eeprom, if no previous config was saved it will wait in setup for the serial input it expects.
		* it will only set the config if it gets a full block
		* user can recofig at anytime from loop() including a clear command to reset it
		* config will be dumped on initial startup, after reconfig or on command showcfg:
		* eeprom required for config struct 89 bytes
*/

/*

example config block serial data expected:
-------------------
	uid:1\n
	apikey:apikey\n
	server:server\n
	passwd:pass\n
	ssid:ssid\n
	enc:0\n

other commands:
-------------------
    clear:\n
	showcfg:\n

*/


#include <EEPROMex.h>
 
uint32_t MAGIC = 0xBAADD00D;

struct settings{
	uint32_t magic;
	uint32_t uid;
	uint8_t encType;
	char apikey[20];
	char server[20];
	char pass[20];
	char ssid[20];
};

settings config;

void readConfig(struct settings *s)
{

	String n;
	String v;
	int i=0;

	while( Serial.available() ){
		  n = Serial.readStringUntil(':');
		  v = Serial.readStringUntil('\n');	
		  //Serial.print(n + v+"\n");
		  
		  if(n=="uid"){ s->uid = atoi(v.c_str()); i++;}
		  if(n=="enc"){ s->encType  = (uint8_t)atoi(v.c_str()); i++;}
		  if(n=="apikey"){ strncpy(s->apikey, v.c_str(), 20); i++;}
		  if(n=="server"){ strncpy(s->server, v.c_str(), 20); i++;}
		  if(n=="passwd"){ strncpy(s->pass, v.c_str(), 20); i++;}
		  if(n=="ssid"){ strncpy(s->ssid, v.c_str(), 20); i++;}
		  
		  if(n=="clear"){ 
				memset(s,0, sizeof(struct settings));
				EEPROM.writeBlock(0, *s);
				s->magic = MAGIC+1;
				while( Serial.available() ) Serial.read(); //eat any remaining input..
				return;
		  }

		  if(n=="showcfg"){
			  showConfig(s); 
			  while( Serial.available() ) Serial.read(); //eat any remaining input..
  			  return;
		  }

	}

	s->magic = 0;
	if(i==6){
		s->magic = MAGIC; //only if all fields were set complete config assured..
		EEPROM.writeBlock(0, *s);
		showConfig(&config);
	}

}

void showConfig(struct settings *s)
{
	Serial.print("Config Dump:");
	Serial.print(s->uid);
	Serial.print(' ');
	Serial.print(s->encType);
	Serial.print(' ');
	Serial.print(s->apikey);
	Serial.print(' ');
	Serial.print(s->server);
	Serial.print(' ');
	Serial.print(s->pass);
	Serial.print(' ');
	Serial.println(s->ssid);
}


//make sure all serial responses end with \n ! 

  void setup()
  {
	    Serial.begin(9600);
		EEPROM.readBlock(0, config);
		
failed:
		if(config.magic != MAGIC){
			while( !Serial.available() ){
				Serial.print(F("Waiting for configuration...\n"));
				delay(1000);
			}
			readConfig(&config);
			if(config.magic != MAGIC) goto failed;
		}else{
			//we wont get here without a valid config..
			showConfig(&config);
		}

 }

 void loop()
 {

	if( Serial.available() ){
		readConfig(&config);
		if(config.magic == MAGIC+1){
			Serial.print(F("Configuration cleared, restart required\n"));
			while(1);
		}
		else 
			if(config.magic != MAGIC) Serial.println(F("Failed to read complete config.."));
		 
	}
    
	delay(2000);
       

 }
 
 
 
 