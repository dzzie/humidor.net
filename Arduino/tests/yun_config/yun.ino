#include <Bridge.h>
//#include <HttpClient.h>
#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7735.h> // Hardware-specific library 
#include <SPI.h>
#include <FileIO.h>

#define TFT_DC      8
#define TFT_RST     9 
#define TFT_CS     10
#define TFT_MOSI   11 
#define TFT_SCLK   12  

Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_MOSI, TFT_SCLK, TFT_RST);

uint8_t uid = 0;
uint8_t delta_t = 0;
uint8_t delta_h = 0;
char apikey[20]={0};
char hostname[30]={0};
char localIP[25] = {0};
char tmp[34] = {0};

/* sample config.txt file 

6,0,2,test,sandsprite.com

# first line must be the config settings
# the format is userid, deltaT, deltaH, apikey, server  
# no spaces, this data after doent matter.

*/

//serial monitor will be required if you def this..sketch will hang till it connects..
#if 0
   #define IFS(x) x
#else
   #define IFS(x)
#endif

void setup() {
  
  tft.initR(INITR_BLACKTAB);   
  tft.fillScreen(ST7735_BLACK);
  tft.setCursor(0, 0);
   
  tft.println("Starting Bridge...");
  //todo display graphic for long delay..

  pinMode(13, OUTPUT);
  digitalWrite(13, LOW);
  Bridge.begin();
  digitalWrite(13, HIGH);

  FileSystem.begin();

  IFS(Serial.begin(9600);)
  IFS(while(!Serial);)
  IFS(Serial.println("setup complete");)
 

}

void loop() {
  /*HttpClient client;
  client.get("http://arduino.cc/asciilogo.txt");

  while (client.available()) {
    char c = client.read();
    Serial.print(c);
  }*/
  bool configOk = false;
 
  //this config mechanism requies 10% memory..
  if(uid==0){
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
		  //while(1){;}
	  } 
	  if(!configOk){
		  tft.println("Invalid config.");
		  delay(1000);
		  return;
		  while(1);
	  }
	  delay(3000);
	  tft.fillScreen(ST7735_BLACK);
  }
 

	if(localIP[0] == 0){
		if( yun_localIp() ){
			  tft.setCursor(0, 0);
			  tft.print("ip: ");
			  tft.println(localIP);
		}
	}

    


    delay(2000);

 
}

//the linux side of yun takes 60 seconds to boot and for this to return true...(7%)

bool yun_localIp() {
  
  Process p;         
  p.begin("ifconfig");  
  p.run();      

  String output = p.readString();
  int wlan = output.indexOf("wlan0");
  if(wlan < 1) return false;

  int ip = output.indexOf("inet addr:", wlan);
  if(ip < 1) return false;

  ip+=10;
  uint8_t i = 0;
  while(output.charAt(ip) != ' ' && i < 23){
		localIP[i] = output.charAt(ip);
		i++;
		ip++;
  }
  localIP[i] = 0;
  
  IFS(Serial.print("Local Ip: ");)
  IFS(Serial.println(localIP);)

  return true;

}

  /*
  while (p.available()>0) {
    char c = p.read();
    Serial.print(c);
	tft.print(c);
  }
  Serial.flush();*/

 /*
eth1      Link encap:Ethernet  HWaddr B4:21:8A:F8:15:33  
          UP BROADCAST MULTICAST  MTU:1500  Metric:1
          RX packets:0 errors:0 dropped:0 overruns:0 frame:0
          TX packets:0 errors:0 dropped:0 overruns:0 carrier:0
          collisions:0 txqueuelen:1000 
          RX bytes:0 (0.0 B)  TX bytes:0 (0.0 B)
          Interrupt:4 

lo        Link encap:Local Loopback  
          inet addr:127.0.0.1  Mask:255.0.0.0
          UP LOOPBACK RUNNING  MTU:16436  Metric:1
          RX packets:53 errors:0 dropped:0 overruns:0 frame:0
          TX packets:53 errors:0 dropped:0 overruns:0 carrier:0
          collisions:0 txqueuelen:0 
          RX bytes:3637 (3.5 KiB)  TX bytes:3637 (3.5 KiB)

wlan0     Link encap:Ethernet  HWaddr B4:21:8A:F0:15:33  
          inet addr:192.168.0.8  Bcast:192.168.0.255  Mask:255.255.255.0
          UP BROADCAST RUNNING MULTICAST  MTU:1500  Metric:1
          RX packets:114 errors:0 dropped:0 overruns:0 frame:0
          TX packets:119 errors:0 dropped:0 overruns:0 carrier:0
          collisions:0 txqueuelen:1000 
          RX bytes:11848 (11.5 KiB)  TX bytes:13282 (12.9 KiB)
*/


//}

/* if you want to configure over http...  (adds 10%)

//example url to trigger ip: http://192.168.0.8/arduino/ip/ip/

#include <YunServer.h>
#include <YunClient.h>

YunServer server;

setup()
  server.listenOnLocalhost();
  server.begin();

loop()
YunClient client = server.accept();

    if(client)
    {        
        String command;
        command=client.readStringUntil('/');

		if(command == "ip"){
			if( yun_localIp() ){
				client.print("ip ok!");
			}else{
				client.print("ip failed!");
			}
			client.flush();
			client.stop();
			return;
		}

		uid = command.toInt() | 0x000000FF;
		
		command  = client.readStringUntil('/');
        delta_t  = command.toInt() | 0x000000FF;
            
		command  = client.readStringUntil('/');
        delta_h  = command.toInt() | 0x000000FF;

		command  = client.readStringUntil('/');
		if(command.length() < 20){
			strcpy(apikey, command.c_str());
		}     
	}

*/

