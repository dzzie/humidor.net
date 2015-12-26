
Materials:
------------------------------
Adafruit HUZZAH ESP8266 breakout – $10
https://www.adafruit.com/product/2471

USB to TTL Serial Cable - Debug Cable – $10
https://www.adafruit.com/products/954

DHT22 temperature-humidity sensor – $10
https://www.adafruit.com/product/385

5V 2A (2000mA) switching power supply - UL Listed - $8
https://www.adafruit.com/products/276

Female DC Power adapter - 2.1mm jack to screw terminal block $2
https://www.adafruit.com/products/368


about:
------------------------------
this is a low-cost build that uses the Adafruit HUZZAH ESP8266 breakout
total cost is about $40.

It does not contain any type of display or buttons and is very easy to put together
only five wires total and for those our power and ground

not quite complete yet soon to be

the following link will help you set up your Arduino IDE to use this board

https://learn.adafruit.com/adafruit-huzzah-esp8266-breakout/using-arduino-ide


wiring:
------------------------------

for programming and serial output wire TTL serial cable as above. this can also
power it while connected to PC

for standalone you will need to wire VBAT and GND to a 5v power supply such
as the one listed above.

The DHT22 should be wired to the 3v power and ground pins, and pin 2

grand total of 5 wires (and 4 of those are just power and ground connections)


source edits:
------------------------------

rename public.h to private.h and edit with wifi name/password and web user id/pass