/**
 * Modern Vintage CNC Firmware
*/
#pragma once

/**
 * Arduino Mega or Due with RAMPS Duo pin assignments
 *
 * Applies to the following boards:
 *
 *  RAMPS_DUO_EFB (Hotend, Fan, Bed)
 *  RAMPS_DUO_EEB (Hotend0, Hotend1, Bed)
 *  RAMPS_DUO_EFF (Hotend, Fan0, Fan1)
 *  RAMPS_DUO_EEF (Hotend0, Hotend1, Fan)
 *  RAMPS_DUO_SF  (Spindle, Controller Fan)
 *
 *  Differences between
 *  RAMPS_14 | RAMPS_DUO
 *    A9/D63 | A12/D66
 *   A10/D64 | A13/D67
 *   A11/D65 | A14/D68
 *   A12/D66 | A15/D69
 *       A13 | A9
 *       A14 | A10
 *       A15 | A11
 */

#define BOARD_INFO_NAME "RAMPS Duo"

#define ALLOW_SAM3X8E
#include "../ramps/pins_RAMPS.h"

//
// Temperature Sensors
//
#undef TEMP_0_PIN
#define TEMP_0_PIN                             9  // Analog Input

#undef TEMP_1_PIN
#define TEMP_1_PIN                            11  // Analog Input

#undef TEMP_BED_PIN
#define TEMP_BED_PIN                          10  // Analog Input

// SPI for MAX Thermocouple
#undef TEMP_0_CS_PIN
#if DISABLED(SDSUPPORT)
  #define TEMP_0_CS_PIN                       69  // Don't use 53 if using Display/SD card
#else
  #define TEMP_0_CS_PIN                       69  // Don't use 49 (SD_DETECT_PIN)
#endif

//
// LCD / Controller
//
#if HAS_WIRED_LCD

  #if BOTH(IS_NEWPANEL, PANEL_ONE)
    #undef LCD_PINS_D4
    #define LCD_PINS_D4                       68

    #undef LCD_PINS_D5
    #define LCD_PINS_D5                       69

    #undef LCD_PINS_D7
    #define LCD_PINS_D7                       67
  #endif

  #if IS_NEWPANEL

    #if ENABLED(REPRAPWORLD_GRAPHICAL_LCD)

      #undef BTN_EN1
      #define BTN_EN1                         67

      #undef BTN_ENC
      #define BTN_ENC                         66

    #elif ENABLED(MINIPANEL)

      #undef DOGLCD_CS
      #define DOGLCD_CS                       69

      #undef LCD_BACKLIGHT_PIN
      #define LCD_BACKLIGHT_PIN               68  // backlight LED on A14/D68

      #undef KILL_PIN
      #define KILL_PIN                        67

      #undef BTN_EN2
      #define BTN_EN2                         66

    #else

      #if IS_RRW_KEYPAD
        #undef BTN_EN1
        #define BTN_EN1                       67  // encoder

        #undef BTN_ENC
        #define BTN_ENC                       66  // enter button
      #elif ENABLED(PANEL_ONE)
        #undef BTN_EN2
        #define BTN_EN2                       66  // AUX2 PIN 4
      #endif
    #endif

    #if ENABLED(REPRAP_DISCOUNT_FULL_GRAPHIC_SMART_CONTROLLER)
      #define BTN_ENC_EN             LCD_PINS_D7  // Detect the presence of the encoder
    #endif

  #endif // IS_NEWPANEL

#endif // HAS_WIRED_LCD
