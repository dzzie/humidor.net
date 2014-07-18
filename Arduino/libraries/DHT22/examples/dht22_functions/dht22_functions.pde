/*####################################################################
 FILE: dht11_functions.pde - DHT11 Usage Demo.
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


#include <dht22.h>

dht22 DHT22;

void setup()
{
  DHT22.attach(2);
  Serial.begin(9600);
  Serial.println("DHT22 TEST PROGRAM ");
  Serial.print("LIBRARY VERSION: ");
  Serial.println(DHT22LIB_VERSION);
}

void loop()
{
  Serial.println("\n");

  int chk = DHT22.read();

  Serial.print("Read sensor: ");
  switch (chk)
  {
    case 0: Serial.println("OK"); break;
    case -1: Serial.println("Checksum error"); break;
    case -2: Serial.println("Time out error"); break;
    default: Serial.println("Unknown error"); break;
  }

  Serial.print("Humidity (%): ");
  Serial.println((float)DHT22.humidity, DEC);

  Serial.print("Temperature (°C): ");
  Serial.println((float)DHT22.temperature, DEC);

  Serial.print("Temperature (°F): ");
  Serial.println(DHT22.fahrenheit(), DEC);

  Serial.print("Temperature (°K): ");
  Serial.println(DHT22.kelvin(), DEC);

  Serial.print("Dew Point (°C): ");
  Serial.println(DHT22.dewPoint(), DEC);

  Serial.print("Dew PointFast (°C): ");
  Serial.println(DHT22.dewPointFast(), DEC);

  delay(2000);
}
