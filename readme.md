
This code and hardware schmatics are for a temperature 
controlled web enabled humidor. 

This allows you to monitor your investment from anywhere
in the world, and receive immediate alerts if it requires
attention.

A comprehensive help file is now available to walk you through 
the setup:

http://sandsprite.com/humidor/help/

A compiled html help (CHM) file of this information is also 
included in this repository.

About:
----------------------------------------------
The Dht22 sensor is installed in the humidor.

The arduino takes a reading every 30 minutes, and uploads
the data to your webserver.

The PHP script will record the data to the database. If
the temp or humidity is out of desired range, it will send
you an email alert. 

Alerts must be manually cleared latter by logging into the 
web site, so you are not spammed, before you get a chance to fix it.

When you add water, push the select button on the LCD sheild to
record it. When you have a smoke, press the down button. 
These will be saved to the database and displayed in the report.
Power resets will also be recorded to the database.

Some users choose to go headless and omit the lcd shield and buttons.
This is now even easier now that we have a [Windows Phone app](https://www.microsoft.com/en-us/store/apps/iot-humidor/9nblggh6404f) to keep
you in sync. 

I already have the web backend setup for multiple users. If you dont have
a public webserver and would like to use mine just send me an email and a 
picture of your humidor. I will set you up with a clientID and api key you 
can use.

By default it will use an internal watchdog to detect hangs in the wifi card.
It may not catch all and you may still end up with hangs. You can optionally
use an external watchdog for another $16 see reference [6]. 

If you build one of these, I would love to see your setup. Please email a 
picture of the humidor and enclosure to dzzie@yahoo.com. I will add a 
users folder to showcase them. 

* Live feed: http://sandsprite.com/humidor/index.php?id=6

See screen shot for example web report.

![screenshot](https://raw.githubusercontent.com/dzzie/humidor.net/master/screenshot.png)
 
Build Videos:
----------------------------------------------
* Hardware: https://www.youtube.com/watch?v=eM54fs1qsvk&list=UUhIoXVvn4ViA3AL4FJW8Yzw
* Haier reset default temp: http://sandsprite.com/blogs/index.php?uid=10&pid=317
* Vinotemp reset default temp: http://sandsprite.com/blogs/index.php?uid=10&pid=316

Status: complete

Materials:
----------------------------------------------
  * [1] Arduino Uno R3 - $24  
  * [2] DHT22 Temperature/Humidity Sensor - $12
  * [3] AdaFruit 16x2 LCD Shield - $20  
  * [4] AdaFruit Wireless Ethernet Shield - $40 
  * [5] Haier 8 bottle thermoelectric wine cooler - $100
  * [6] External Watchdog (optional) - $16
  * WebServer with Php and MySql support
  * 9-12v 1 Amp DC center positive power adapter w/ 2.1mm plug 
  * Total Cost: $200

Note: Make sure your Arduino is powered by a 1 amp or higher rated external power supply
when using with the CC3000! Less will lead to unstable behavior and lockups!

Pins used and assembly:
----------------------------------------------

dht22

    * 5V, GND, digital pin #2

SPI wifi shield (solder pass through headers on and plug in)

    * SCK  |pin #13|
    * MISO |pin #12|
    * MOSI |pin #11|
    * CS for CC3000 |pin #10|
    * VBAT_EN |pin #5|
    * CS for SD Card |pin #4|
    * IRQ |pin #3|

I2C lcd shield (soldered from kit and plug in)

   * 5v, gnd, ic2 pins:
   * SCL |Analog pin #4| 
   * SDA |Analog pin #5|

   Buttons Used:
      * Left = force update
      * Down = smoked
      * select = watered
      * reset

external watchdog (optional [6])

	* digital pin 6      
      
autowater removed see [7]

Note: The dht22 sensor will burn out if you hook it up backwards. 
    It is a 4 pin component with one pin unused. I cut off the unused 
    pin, and plugged the matching hole in the socket with hot glue. 
    This makes it so that the sensor can only be inserted in the correct orientation.

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

[6] SwitchDoc Labs Dual WatchDog Timer (optional)
http://www.amazon.com/SwitchDoc-Labs-WatchDog-Arduino-Raspberry/dp/B00OL1N7R2/ref=sr_1_2?ie=UTF8&qid=1442439681&sr=8-2&keywords=dual+watchdog

[7] auto water feature 
This feature was removed in commit 9b07772. For my humidor I added 5 large boveda packs
and a pound of beads just for use as a buffer. I would be lucky if I went through 5 packs a year.
These give rock solid stability and no more ups and downs with watering. If you are spending this much
on your humidor to do this system, just use boveda. If you really want to try the autowater, you will 
additionally need a parastolic pump, and a motor driver board. I never actually used it in production.

pins used:
	* pump activation d6
    * switchIn A0
    * switchOut A1
   
   