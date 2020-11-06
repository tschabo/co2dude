#pragma once

////////////////
// CO2 Sensor //
////////////////
//#define ENABLE_MHZ19
#define ENABLE_SCD30

//////////////
// Features //
//////////////
#define ENABLE_SD_LOGGING
#define ENABLE_RTC
#define ENABLE_DISPLAY
#define ENABLE_LEDSTRIP

// MHZ19 Settings
#ifdef ENABLE_MHZ19
// at the moment only SoftwareSerial
#define CO2_SERIAL_RX_PIN D4
#define CO2_SERIAL_TX_PIN D0
#endif

//////////
// LEDs //
//////////

#define PIXEL_PIN D3 // LED.
#define PIXEL_COUNT 1
