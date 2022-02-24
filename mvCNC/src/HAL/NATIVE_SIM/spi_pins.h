/**
 * Modern Vintage CNC Firmware
 * Copyright (c) 2022 Edward Webber [https://github.com/Domush/mvCNC-Modern-Vintage-CNC-Firmware]

 * Based on Marlin and grbl.
 */
#pragma once

#include "../../core/macros.h"
#include "../../inc/mvCNCConfigPre.h"

#if BOTH(HAS_MVCNCUI_U8GLIB, SDSUPPORT) && (LCD_PINS_D4 == SD_SCK_PIN || LCD_PINS_ENABLE == SD_MOSI_PIN || DOGLCD_SCK == SD_SCK_PIN || DOGLCD_MOSI == SD_MOSI_PIN)
  #define LPC_SOFTWARE_SPI  // If the SD card and LCD adapter share the same SPI pins, then software SPI is currently
                            // needed due to the speed and mode required for communicating with each device being different.
                            // This requirement can be removed if the SPI access to these devices is updated to use
                            // spiBeginTransaction.
#endif

// Onboard SD
//#define SD_SCK_PIN     P0_07
//#define SD_MISO_PIN    P0_08
//#define SD_MOSI_PIN    P0_09
//#define SD_SS_PIN      P0_06

// External SD
#ifndef SD_SCK_PIN
  #define SD_SCK_PIN        50
#endif
#ifndef SD_MISO_PIN
  #define SD_MISO_PIN       51
#endif
#ifndef SD_MOSI_PIN
  #define SD_MOSI_PIN       52
#endif
#ifndef SD_SS_PIN
  #define SD_SS_PIN         53
#endif
#ifndef SDSS
  #define SDSS       SD_SS_PIN
#endif
