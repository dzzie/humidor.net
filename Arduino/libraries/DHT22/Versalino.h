/*####################################################################
 FILE: Versalino.h 
 VERSION: 1S0B
 PURPOSE: Library for the ATMEGA328 based Versalino.
 LICENSE: GPL v3 (http://www.gnu.org/licenses/gpl.html)

 DATASHEET: https://www.virtuabotix.com/resources.php?product=versalino
 GET UPDATES: https://www.virtuabotix.com/resources.php?product=versalino
                                                                                                                                                                                                                       
 HISTORY:
 Structure Improvements Joseph Dattilo (Virtuabotix LLC) - Version 1S0B (04/10/12)
 Joseph Dattilo (Virtuabotix LLC) - Version 1S0A (04/01/12)
#######################################################################*/


#ifndef VERSALINO_H
#define VERSALINO_H

#include <stddef.h>

#if defined(ARDUINO) && ARDUINO >= 100
  #include "Arduino.h"
#else
  #include "WProgram.h"
  #include <pins_arduino.h>
#endif

//------------------------type structures----------------------

typedef struct {
	char ID;
	uint8_t PINS[9];
	uint8_t AN0;
	uint8_t AN1;
	uint8_t AN2;
	uint8_t D1;
	uint8_t P1;
	uint8_t D2;
	uint8_t P2;
	uint8_t D3;
	uint8_t P3;
} VersalinoBUS;
 typedef struct {
	uint8_t BUSA;
	uint8_t BUSB;
} VersalinoPIN;

	//---------------single pin identifiers---------------------
	static const int TX = 1;
	static const int RX = 0;

	static const uint8_t AN0=0;
	static const uint8_t AN1=1;
	static const uint8_t AN2=2;
	static const uint8_t D1=3;
	static const uint8_t P1=4;
	static const uint8_t D2=5;
	static const uint8_t P2=6;
	static const uint8_t D3=7;
	static const uint8_t P3=8;

	//---------------Multi-BUS Identifiers------------------------
	static const VersalinoPIN PINS[] =
	{(VersalinoPIN) {A0, A3},
	 (VersalinoPIN) {A1, A4},
	 (VersalinoPIN) {A2, A5},
	 (VersalinoPIN) {2,  7},
	 (VersalinoPIN) {3,  9},
	 (VersalinoPIN) {4,  8},
	 (VersalinoPIN) {5,  10},
	 (VersalinoPIN) {13, 12},
	 (VersalinoPIN) {6,  11}
	};

	//---------------Full BUS Layouts
	static const VersalinoBUS BUSA={ 'A',
									{PINS[AN0].BUSA,PINS[AN1].BUSA,PINS[AN2].BUSA,PINS[D1].BUSA,PINS[P1].BUSA,PINS[D2].BUSA,PINS[P2].BUSA,PINS[D3].BUSA,PINS[P3].BUSA},
									 PINS[AN0].BUSA,PINS[AN1].BUSA,PINS[AN2].BUSA,PINS[D1].BUSA,PINS[P1].BUSA,PINS[D2].BUSA,PINS[P2].BUSA,PINS[D3].BUSA,PINS[P3].BUSA
								   };

	static const VersalinoBUS BUSB={ 'B',
									{PINS[AN0].BUSB,PINS[AN1].BUSB,PINS[AN2].BUSB,PINS[D1].BUSB,PINS[P1].BUSB,PINS[D2].BUSB,PINS[P2].BUSB,PINS[D3].BUSB,PINS[P3].BUSB},
									 PINS[AN0].BUSB,PINS[AN1].BUSB,PINS[AN2].BUSB,PINS[D1].BUSB,PINS[P1].BUSB,PINS[D2].BUSB,PINS[P2].BUSB,PINS[D3].BUSB,PINS[P3].BUSB
								   };



#endif // VERSALINO_H
