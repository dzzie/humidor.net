#ifndef PRIVATE_H
#define PRIVATE_H

//rename this file to private.h and change the settings to match your setup.
//my private files have been excluded for obvious reasons.
#define USE_EXT_WATCHDOG 0
#define WLAN_SSID       "AccessPoint Name"           // cannot be longer than 32 characters!
#define WLAN_SECURITY   WLAN_SEC_WEP      // Security can be WLAN_SEC_UNSEC, WLAN_SEC_WEP, WLAN_SEC_WPA or WLAN_SEC_WPA2

const char WLAN_PASS[] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00}; //this is the WEP format for a key..
char* APIKEY = "123456";

//in case your sensor reading is continiously low or high from another calibrated trusted sensor
#define temp_shift   0   
#define humi_shift   0 
#define client_id    0 
//set client_id unique for each humidor so web page can display multiple ones. 66 is used for test data..

#endif // PRIVATE_H
