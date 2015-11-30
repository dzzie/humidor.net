#include <stdlib.h>
#include <string.h>
#include <conio.h>
#include <stdio.h>

#define uint8_t unsigned char

char respCode[16];
char pageResp[16];

char* h1 = "HTTP/1.1 200 OK\r\n" 
"Date: Sat, 26 Jul 2014 02:22:02 GMT\r\n" 
"Server: Apache/2.2.11 (Win32) PHP/5.3.0\r\n" 
"X-Powered-By: PHP/5.3.0\r\n"
"Content-Length: 20\r\n"
"Connection: close\r\n"
"Content-Type: text/html\r\n"
"\r\n"
"<h1>Invalid api key!\r\n\r\n";

char* h2 = "HTTP/1.1 200 OK\n" 
"Date: Sat, 26 Jul 2014 02:22:02 GMT\n" 
"Server: Apache/2.2.11 (Win32) PHP/5.3.0\n" 
"X-Powered-By: PHP/5.3.0\n"
"Content-Length: 20\n"
"Connection: close\n"
"Content-Type: text/html\n"
"\n"
"<h1>Invalid api key!\n\n";

//ip string to ip
unsigned int ips2ip(char* ips){
  int i=0, j=0, k=0;
  uint8_t b[4]={0,0,0,0};
  char tmp[5] = {0,0,0,0,0};

  while(*ips){
	  if(*ips=='.'){
		  k = atoi(tmp);
		  if( k > 255 ) return 0; //invalid byte value
		  b[j++] = k;
		  for(; i>=0; i--) tmp[i]=0; //wipe buffer
		  i=0;
		  if(j == 4) return 0; //to many dotted sections
	  }else{
		 tmp[i++] = *ips;
		 if( i > sizeof(tmp)-1 ) return 0; //max 3 chars exceeded
	  }
	  ips++;
  }

  if(tmp[0] != 0){
	  k = atoi(tmp);
	  if( k > 255 ) return 0;
	  b[j++] = k;
  }

  if(j != 4) return 0;
  return 1;
  //return cc3000.IP2U32(b[0], b[1], b[2], b[3]);
   
}

void main (void){

  printf("%x", ips2ip("192.168.0.10"));
  printf("%x", ips2ip("19211111.168.0.10"));
  printf("%x", ips2ip("aaaa192.168.0.10"));
  printf("%x", ips2ip("192 168.0.10"));
  printf("%x", ips2ip("tacoaaa"));

  getch();
  return; 

  int nl_count = 0;
  int rc_offset = 0;
  int pr_offset = 0;
  int recording = 0;
  char respCode[16] = {0};
  char pageResp[16] = {0};
  int rCode = 0;

  char c;
  char* w = h2;

  while (*w) {
     
        c = *w;
 
		//this accounts for \r\n or just \n
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
			if(recording == 2) pageResp[pr_offset++] = c; 
		}
        
		w++;
  }
  pageResp[pr_offset]=0;
  respCode[rc_offset]=0;

  rCode = atoi(respCode);

}