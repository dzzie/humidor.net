
This code and hardware schmatics are for a
temperature controlled web enabled humidor.  

Status: in development

Materials:
----------------------------------------------
  * [1] Arduino Uno R3 - $24  
  * [2] DHT22 Temperature/Humidity Sensor - $12
  * [3] AdaFruit 16x2 LCD Shield - $20  
  * [4] AdaFruit Wireless Ethernet Shield - $40 
  * [5] Haier 8 bottle thermoelectric wine cooler - $100
  * WebServer with Php and MySql support
  * arduino power adapter 
  * Total Cost: $200


About:
----------------------------------------------
The Dht22 sensor is installed in the humidor.

The arduino takes a reading every 20 minutes, and uploads
the data to your webserver.

The PHP script will record the data to the database. If
the temp or humidity is out of desired range, it will send
you an email alert. 

Alerts must be manually cleared latter by logging into the 
web site, so you are not spammed, before you get a chance to fix it.

When you add water, push the watered button on the LCD sheild to
record it. This will be saved to the db as well. 

See screen shot for example web report.

![screenshot](http://raw.github.com/dzzie/humidor.net/screenshot.png)
 

Pins used and assembly:
----------------------------------------------

dht22
    5V, GND, library default: digital pin #2

wifi shield (solder pass through headers on and plug in)
    SCK  pin #13
    MISO pin #12
    MOSI pin #11
    CS for CC3000 pin #10
    VBAT_EN pin #5
    CS for SD Card pin #4
    IRQ pin #3

lcd shield (soldered from kit and plug in)
 5v, gnd 
 ic2 pins:
   SCL Analog pin #4 
   SDA Analog pin #5


Links:
---------------------------------------------
[1] Arduino Uno R3
http://www.adafruit.com/products/50

[2] DHT22 Temperature/Humidity Sensor 
https://www.virtuabotix.com/product/virtuabotix-dht22-temperature-humidity-sensor-arduino-microcontroller-circuits/

[3] AdaFruit 16x2 LCD Shield
http://www.adafruit.com/products/772

[4] AdaFruit Wireless Ethernet Shield
http://www.adafruit.com/products/1491
https://learn.adafruit.com/adafruit-cc3000-wifi/cc3000-shield

[5] Haier 8 bottle thermoelectric wine cooler
http://www.amazon.com/Haier-8-Bottle-Bottle-Electronic-Controls/dp/B00DNSO2BO/ref=sr_1_1?ie=UTF8&qid=1405699936&sr=8-1&keywords=haier+wine+fridge
