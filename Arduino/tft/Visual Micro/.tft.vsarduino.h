/* 
	Editor: http://www.visualmicro.com
	        visual micro and the arduino ide ignore this code during compilation. this code is automatically maintained by visualmicro, manual changes to this file will be overwritten
	        the contents of the Visual Micro sketch sub folder can be deleted prior to publishing a project
	        all non-arduino files created by visual micro and all visual studio project or solution files can be freely deleted and are not required to compile a sketch (do not delete your own code!).
	        note: debugger breakpoints are stored in '.sln' or '.asln' files, knowledge of last uploaded breakpoints is stored in the upload.vmps.xml file. Both files are required to continue a previous debug session without needing to compile and upload again
	
	Hardware: Arduino Mega w/ ATmega2560 (Mega 2560), Platform=avr, Package=arduino
*/

#ifndef _VSARDUINO_H_
#define _VSARDUINO_H_
#define __AVR_ATmega2560__
#define ARDUINO 158
#define ARDUINO_MAIN
#define __AVR__
#define __avr__
#define F_CPU 16000000L
#define __cplusplus
#define __inline__
#define __asm__(x)
#define __extension__
#define __ATTR_PURE__
#define __ATTR_CONST__
#define __inline__
#define __asm__ 
#define __volatile__

#define __builtin_va_list
#define __builtin_va_start
#define __builtin_va_end
#define __DOXYGEN__
#define __attribute__(x)
#define NOINLINE __attribute__((noinline))
#define prog_void
#define PGM_VOID_P int
            
typedef unsigned char byte;
extern "C" void __cxa_pure_virtual() {;}

void cls();
void setup(void);
void loop(void);
void watchdogReset();
void watchdogDisable();
void watchdogEnable();
void show_readings(bool drawButtons);
void lcd_out(char* s);
void lcd_outp(const __FlashStringHelper *s);
void delay_x_min(int minutes);
void lcd_out(char* s, int row);
void lcd_outp(const __FlashStringHelper *s, int row);
void displayFlags(int min);
void delay_x_min(int minutes, int silent);
bool ReadSensor();
unsigned long timeDiff(unsigned long startTime);
void lcd_ip_out(uint32_t ip);
bool PostData();
double toFahrenheit(double dCelsius);
int dht22_read(uint8_t pin);
void bmpDraw(char *filename, uint8_t x, uint16_t y);
uint16_t read16(File &f);
uint32_t read32(File &f);
static int myini_handler(void* user, const char* section, const char* name, const char* value);
char* my_fgets(char* buf, int len, File* stream);
bool loadConfig();
void hexDump(char* input);
int HexToBin(char* input);
void showWifiCfg();
void showCfg(bool blockTillTouch);
void ps(char* name, char* value);
void ps(char* name, uint16_t value);
uint32_t ips2ip(char* ips);

#include "D:\arduino-1.5.8\hardware\arduino\avr\cores\arduino\arduino.h"
#include "D:\arduino-1.5.8\hardware\arduino\avr\variants\mega\pins_arduino.h" 
#include "d:\_code\humidor.net\Arduino\tft\tft.ino"
#include "d:\_code\humidor.net\Arduino\tft\ini.c"
#include "d:\_code\humidor.net\Arduino\tft\ini.h"
#endif
