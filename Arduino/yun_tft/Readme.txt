
this is a test project to see if we can simplify the build.

this one uses an arduino yun ($75). Display is handled by a 
$20 - 1.8" Color TFT LCD display with MicroSD Card Breakout - ST7735R

https://www.adafruit.com/products/358

You will additionally need the following libraries installed:

#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7735.h> // Hardware-specific library 

https://github.com/adafruit/Adafruit-ST7735-Library
https://github.com/adafruit/Adafruit-GFX-Library

The display is in the vertical orientation (long way vertical)

config settings are read in from a MicroSD card on the yun.
the file must be named config.txt in the root of the drive and
the data is in the following format:

# first line must be the config settings
# the format is userid, deltaT, deltaH, apikey, server  
# no spaces, this data after doent matter.
# ex: 

6,0,2,test,sandsprite.com

