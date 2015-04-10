/* 
	Editor: http://www.visualmicro.com
	        visual micro and the arduino ide ignore this code during compilation. this code is automatically maintained by visualmicro, manual changes to this file will be overwritten
	        the contents of the Visual Micro sketch sub folder can be deleted prior to publishing a project
	        all non-arduino files created by visual micro and all visual studio project or solution files can be freely deleted and are not required to compile a sketch (do not delete your own code!).
	        note: debugger breakpoints are stored in '.sln' or '.asln' files, knowledge of last uploaded breakpoints is stored in the upload.vmps.xml file. Both files are required to continue a previous debug session without needing to compile and upload again
	
	Hardware: Arduino Uno, Platform=avr, Package=arduino
*/

#ifndef _VSARDUINO_H_
#define _VSARDUINO_H_
#define __AVR_ATmega328p__
#define __AVR_ATmega328P__
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

void setup(void);
void loop(void);
void show_readings();
void lcd_out(char* s);
void lcd_out(char* s, int row);
void lcd_ip_out(uint32_t ip, int row);
void delay_x_min(int minutes);
void delay_x_min(int minutes, int silent);
bool ReadSensor();
void showProgress(void);
bool PostData();
double toFahrenheit(double dCelsius);
int dht22_read(uint8_t pin);

#include "d:\arduino-1.5.8\hardware\arduino\avr\cores\arduino\arduino.h"
#include "d:\arduino-1.5.8\hardware\arduino\avr\variants\standard\pins_arduino.h" 
#include "d:\_code\humidor.net\Arduino\humidor\humidor.ino"
#include "d:\_code\humidor.net\Arduino\humidor\private.h"
#include "d:\_code\humidor.net\Arduino\humidor\public.h"
#endif
