/**
 * Modern Vintage CNC Firmware
 * Copyright (c) 2022 Edward Webber [https://github.com/Domush/mvCNC-Modern-Vintage-CNC-Firmware]

 * Based on Marlin and grbl.
 */
#pragma once

/**
 * Geeetech A20 GT2560 V4.x board pin assignments
 */

#define BOARD_INFO_NAME "GT2560 4.x"

#define LCD_PINS_RS                            5
#define LCD_PINS_ENABLE                       36
#define LCD_PINS_D4                           21
#define LCD_PINS_D7                            6

#define SPEAKER                                  // The speaker can produce tones

#if IS_NEWPANEL
  #define BTN_EN1                             16
  #define BTN_EN2                             17
  #define BTN_ENC                             19
#endif

#include "pins_GT2560_V3.h"
