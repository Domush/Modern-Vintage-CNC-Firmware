/**
 * Modern Vintage CNC Firmware
 *
 * Copyright (c) 2022 Edward Webber [https://github.com/Domush/mvCNC-Modern-Vintage-CNC-Firmware]

 * Based on Marlin and grbl.
 */
#pragma once

#ifdef ADAFRUIT_GRAND_CENTRAL_M4

 /*
  * AGCM4 Default SPI Pins
  *
  *         SS    SCK   MISO   MOSI
  *       +-------------------------+
  *  SPI  | 53    52     50     51  |
  *  SPI1 | 83    81     80     82  |
  *       +-------------------------+
  * Any pin can be used for Chip Select (SD_SS_PIN)
  */
  #ifndef SD_SCK_PIN
    #define SD_SCK_PIN    52
  #endif
  #ifndef SD_MISO_PIN
    #define SD_MISO_PIN   50
  #endif
  #ifndef SD_MOSI_PIN
    #define SD_MOSI_PIN   51
  #endif
  #ifndef SDSS
    #define SDSS          53
  #endif

#else

  #error "Unsupported board!"

#endif

#define SD_SS_PIN     SDSS
