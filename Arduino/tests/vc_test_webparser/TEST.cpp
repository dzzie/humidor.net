#include <stdlib.h>

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

void main (void){

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