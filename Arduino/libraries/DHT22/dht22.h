/*####################################################################
 FILE: dht11.h - Library for the Virtuabotix DHT22 Sensor.
 VERSION: 2S0A

 PURPOSE: Measure and return temperature & Humidity. Additionally provides conversions.

 LICENSE: GPL v3 (http://www.gnu.org/licenses/gpl.html)
 GET UPDATES: https://www.virtuabotix.com/

      --##--##--##--##--##--##--##--##--##--##--
      ##  ##  ##  ##  ##  ##  ##  ##  ##  ##  ##
      ##  ##  ##  ##  ##  ##  ##  ##  ##  ##  ##
      | ##  ##  ##  ##  ##  ##  ##  ##  ##  ## |
      ##  ##  ##  ##  ##  ##  ##  ##  ##  ##  ##
      ##  ##  ##  ##  ##  ##  ##  ##  ##  ##  ##
      | ##  ##  ##  ##  ##  ##  ##  ##  ##  ## |
      ##  ##  ##  ## DHT22 SENSOR ##  ##  ##  ##
      ##  ##  ##  ##  ##FRONT ##  ##  ##  ##  ##
      | ##  ##  ##  ##  ##  ##  ##  ##  ##  ## |
      ##  ##  ##  ##  ##  ##  ##  ##  ##  ##  ##
      ##  ##  ##  ##  ##  ##  ##  ##  ##  ##  ##
      | ##  ##  ##  ##  ##  ##  ##  ##  ##  ## |
      ##  ##  ##  ##  ##  ##  ##  ##  ##  ##  ##
      ##  ##  ##  ##  ##  ##  ##  ##  ##  ##  ##
      --##--##--##--##--##--##--##--##--##--##--
          ||       ||          || (Not    ||
          ||       ||          || Used)   ||
        VDD(5V)   Readout(I/O)          Ground

  HISTORY:
  Joseph Dattilo (Virtuabotix LLC) - Version 1S0A (14 Sep 12)
    -Converted from DHT11 2S0A library to work with the DHT22
     Portions of DHTLib used for verification, and data manipulation

#######################################################################*/


#ifndef DHT22_H
#define DHT22_H
#define DHT22LIB_VERSION "1S0A"

#include <stddef.h>

#if defined(ARDUINO) && ARDUINO >= 100
  #include "Arduino.h"
#else
  #include "WProgram.h"
  #include <pins_arduino.h>
#endif


#include <Versalino.h>
#include <inttypes.h>


class dht22
{

	public:
	//-------Versalino Functions
	VersalinoBUS getBUS(); //This will return the VersalinoBUS currently assigned to the instance of this library
	void setBUS(VersalinoBUS myBUS);
	void removeBUS();

	//-------dht22 Functions
	dht22();//included for legacy compatability
    dht22(int pin);
    dht22(int pin, VersalinoBUS myBUS);

	void attach(int pin);//Attaches dht11 object to a particular sensorPin
	void attach(int pin, VersalinoBUS myBUS);//Attaches dht11 object to a particular sensorPin on a particular VersalinoBUS

	int read();//defaults to the attached pin
	int read(int pin);
	int read(int pin, VersalinoBUS myBUS);
	double humidity;
	double temperature;
	double celcius();
	double fahrenheit();
	double fahrenheit(double dCelcius);
	double kelvin();
	double kelvin(double dCelcius);
	double dewPoint();
	double dewPointFast();


	private:
	VersalinoBUS _myBUS;
	bool _BUSenabled;
	int _sensorPin;//defaults to pin 2

};


#endif // DHT11_H
