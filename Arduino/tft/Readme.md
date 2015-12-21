
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

Total: $144 


Details
------------------------
The TFT touch shield just plugs right into the mega, it comes fully
assembled. You will have to solder 3 jumpers on the back, and cut 3
traces. The touch screen eliminates the need to mount and wire buttons. 

This build will cost an additional $40 dollars over the LCD version
but is much less work, and will be easier to build an enclosure for.

Details here:
https://learn.adafruit.com/adafruit-2-8-tft-touch-shield-v2/connecting#using-with-a-mega-slash-leonardo

TFT Pins used: (for reference, no wiring required just plug in)

    *Touch:   I2C  SDA, SCL 
    *TFT:     ICSP SCLK/MISO/MOSI, Digital #10 (CS), Digital #9 (DC)
    *MicroSD: ICSP SCLK/MISO/MOSI  Digital #4 (CS)

You can not use the CC3000 Shield due to pin conflicts. The cc3000 breakout
actually requires less soldering. Just wire as follows:

    *Connect GND to one of the Arduino GND pins:
    *Connect Vin to Arduino +5V
    *CS to Digital 48
    *VBAT  49
    *MISO to Digital 50
    *MOSI to Digital 51
    *CLK to Digital 52
    *IRQ   21  

The DHT22 sensor requires 3 wires:

    *Connect GND to one of the Arduino GND pins:
    *Connect Vin to Arduino +5V
    *Digital 46

If your counting, there are only 7 wires to hookup, (and
two sets of power/ground connections)

Next edit public.ini and then save as private.ini. 
Now copy the files from the SDCard_Files folder
to the root of the sd card and insert it into the tft shield.

You will additionally need the following libraries installed:

#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_FT6206.h> // capacitive touch library
#include <Adafruit_ILI9341.h>// Hardware-specific library 

https://github.com/adafruit/Adafruit-GFX-Library
https://github.com/adafruit/Adafruit_FT6206_Library
https://github.com/adafruit/Adafruit_ILI9341

The display is in the vertical orientation (long way vertical)


Notes:
------------------------------

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


Other Credits:
------------------------------

uses Ben Hoyt's Ini file library: 
  https://github.com/benhoyt/inih


