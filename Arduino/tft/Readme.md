
Parts
------------------------

2.8 TFT Touch Shield for Arduino w-Capacitive Touch ID- 1947 - $45 
https://www.adafruit.com/products/1947

Arduino Mega $46
https://www.adafruit.com/products/191

CC3000 Breakout - Adafruit CC3000 WiFi - Adafruit $35
https://learn.adafruit.com/adafruit-cc3000-wifi/cc3000-breakout

DHT22 temperature-humidity sensor + extras $10
https://www.adafruit.com/products/385

4GB Blank SD/MicroSD Memory Card $8
https://www.adafruit.com/product/102

male to female jumpers - $4
https://www.adafruit.com/products/826

Power supply - $7
https://www.adafruit.com/product/63

stacking headers (optional) - $2
https://www.adafruit.com/products/85

Total: $160 

Details
------------------------
This build was created for the following reasons:

	* less soldering, just plug parts together
	* do not need to edit source code with user settings (edit text file on sd card)
	* better looking display, more features
	* easier to build an enclosure for

The TFT touch shield just plugs right into the mega, it comes fully
assembled. You will have to solder 3 jumpers pads on the back, and cut 3
traces. (\hardware_picts\tft_solder.jpg) 

The touch screen eliminates the need to mount and wire buttons
greatly simplifying the enclosure fabrication.  

This build will cost an additional $50 dollars over the LCD version
but is much less work.

TFT Touchscreen Pins used: (for reference, no wiring required just plug in)

    *Touch:   I2C  SDA, SCL 
    *TFT:     ICSP SCLK/MISO/MOSI, Digital #10 (CS), Digital #9 (DC)
    *MicroSD: ICSP SCLK/MISO/MOSI  Digital #4 (CS)

You can not use the CC3000 Shield due to pin conflicts. The cc3000 breakout
actually requires less soldering. Just wire the breakout as follows:

    *Connect GND to one of the Arduino GND pins:
    *Connect Vin to Arduino +5V
    *CS to Digital 48
    *VBAT  49
    *MISO to Digital 50
    *MOSI to Digital 51
    *CLK to Digital 52
    *IRQ   19  (changed since pictures taken)

(Technically you can probably use a shield version if you wire it as a 
breakout, or cut a trace and rewire it as is but your own your own there :)

The DHT22 sensor requires 3 wires:

    *Connect GND to one of the Arduino GND pins:
    *Connect Vin to Arduino +5V
    *Digital 46

If your counting, there are only 7 wires to hookup, (and
two sets of power/ground connections)

If you want to get up and running quickly, buy a pack
of the male to female jumpers. 

All of the connections can be a simple matter of just 
plugging parts together. 

There is very little soldering required in this build. 
(just the 8 pins on the wifi board header, and the 3 pads on
the tft shield)

In the build pictures (\hardware_picts\tft) you can see I 
decided to use a scrap of circuit board and solder the wires
on. This was just my preference for a compact layout. The
jumpers approach will work fine as well. Just be aware a wire may
come loose if it gets jostled around allot.

Next edit public.ini and then save as private.ini. 
Now copy the files from the SDCard_Files folder
to the root of the sd card and insert it into the tft shield.

You will additionally need the following libraries installed:

https://github.com/adafruit/Adafruit-GFX-Library
https://github.com/adafruit/Adafruit_FT6206_Library
https://github.com/adafruit/Adafruit_ILI9341

The display is in the vertical orientation (long way vertical)


Notes:
------------------------------

if you want to be able to use reload cfg and hot swap the sd card while running
you need to patch your SD.cpp file in your arduino libraries directory.

https://github.com/arduino/Arduino/issues/3607

boolean SDClass::begin(uint8_t csPin) {
   if (root.isOpen()) root.close();      // allows repeated calls
   
At the bottom of the main screen there are three icons. A cigar, gear and water drop.
Pressing the cigar toggles the smoked flag and an S will appear on the screen.
Pressing the water drop toggles the watered flag and a W will appear
Pressing the gear will enter the config screen.

The config screen has two tabs and an X in the corner to exit the config mode.
On the top of the config screen, there are the words View | Edit | X
These are tabs. The active tab is white text.

View allows you to see a full config dump. If you are in demo mode, passwords will
be displayed as ******.

On the edit tab you will see the words

local = x
demo  = x
speed = x

You can touch any of these lines to toggle them. Initially they are the values from the 
ini file on the sd card. Local refers to debug_local setting. demo is demo mode, and
speed is speed mode (accelerated count down).

On initial startup, if you touch the screen during the config dump, it will automatically enter
the interactive config mode so you can change settings at startup before any uploads have occured.

This mode is mostly for development, testing, and demo purposes. (touch screens gives you so much
flexibility I just had to add it)

One other bug I noticed, I was using a 12v power supply for a while in testing. The regulator chip
got pretty hot and within an hour the arduino just kept rebooting. Probably a hot glitch. If you
run into problems like this try a lower voltage power supply.

todo:
------------------------------



Other Credits:
------------------------------

uses Ben Hoyt's Ini file library: 
  https://github.com/benhoyt/inih


