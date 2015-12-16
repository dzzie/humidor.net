#include <stdio.h>
#include <conio.h>
#include "ini.h"

//we also use this a a code generator..

static int myini_handler(void* user, const char* section, const char* name, const char* value)
{
	printf("[%s] %s = %s\n", section, name, value);
	//printf("char* %s = \"%s\";\n", name, name);
	//printf("if (strcmp(name, sCfg.%s)==0) cfg.%s = strdup(value);\n", name, name);
	//printf("if(cfg.%s==0){ps(sCfg.missing, sCfg.%s);fail++;}\n", name, name);

    return 1;
}

void main(void){

  
  char* f = "test2.ini";
  //f = "D:\\_code\\humidor.net\\Arduino\\tft\\SDCard_Files\\private.ini";
  
  if (ini_parse(f, myini_handler, 0) < 0) {
       printf("No Ini file");
  }

  getch();

}