

humidor folder contains the arduino project using the 16x2 lcd
this is the main stable dev branch.

--------------------------------------------------------------

the tft folder may be the next version. I am trying to make it 
easier to replicate.

This version uses:

2.8 TFT Touch Shield for Arduino w-Capacitive Touch ID- 1947 - $45 
https://www.adafruit.com/products/1947

Arduino Mega $46
https://www.adafruit.com/products/191

CC3000 Breakout - Adafruit CC3000 WiFi - Adafruit $35
https://learn.adafruit.com/adafruit-cc3000-wifi/cc3000-breakout

DHT22 temperature-humidity sensor + extras $10
https://www.adafruit.com/products/385

The TFT touch shield just plugs right into the mega, it comes fully
assembled. You will have to solder 3 jumpers on the back, and cut 3
traces.

https://learn.adafruit.com/adafruit-2-8-tft-touch-shield-v2/connecting#using-with-a-mega-slash-leonardo

TFT Pins used: (no wiring just plug in)

    Touch:   SDA, SCL 
    TFT:     ICSP SCLK/MISO/MOSI, Digital #10 (CS), Digital #9 (DC)
    MicroSD: ICSP SCLK/MISO/MOSI  Digital #4 (CS)

You can not use the CC3000 Shield due to pin conflicts. The cc3000
actually requires less soldering. Just attach an 8 pin header and wire
as follows:

    Connect GND to one of the Arduino GND pins:
    Connect Vin to Arduino +5V
    CLK to Digital 52
    MISO to Digital 50
    MOSI to Digital 51
    CS to Digital 40
    IRQ   21  
    VBAT  41

You will also need a SD card, copy sir.bmp, cigar.bmp, and water.bmp 
to the root of the sd card. An ini file will also be used (coming soon)

The touch screen eliminates the need to wire buttons. 

This build will cost an additional $40 dollars over the LCD version
but is much less work, and will be easier to build an enclosure for.



