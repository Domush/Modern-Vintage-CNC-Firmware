/**
 * Modern Vintage CNC Firmware
*/
#pragma once

/**
 * VERTEX NANO Arduino Mega with RAMPS EFB v1.4 pin assignments.
 */

#if TOOL_CHANGE_SUPPORT
  #error "Only 1 hotend is supported for Vertex Nano."
#endif

#define BOARD_INFO_NAME      "K8600"
#define DEFAULT_MACHINE_NAME "Vertex Nano"

//
// Limit Switches
//
#define X_MIN_PIN                              3
#define Y_MAX_PIN                             14
#define Z_MAX_PIN                             18
#define Z_MIN_PIN                             -1

//
// Steppers
//
#define Z_ENABLE_PIN                          63

//
// Heaters / Fans
//
#define FAN_PIN                                8

//
// Misc. Functions
//
#define SDSS                                  25
#define CASE_LIGHT_PIN                         7

//
// Other RAMPS pins
//
#include "pins_RAMPS.h"

//
// Heaters / Fans
//
#undef HEATER_BED_PIN

//
// LCD / Controller
//
#if IS_ULTRA_LCD && IS_NEWPANEL
  #undef BEEPER_PIN

  #undef LCD_PINS_RS
  #undef LCD_PINS_ENABLE
  #undef LCD_PINS_D4
  #undef LCD_PINS_D5
  #undef LCD_PINS_D6
  #undef LCD_PINS_D7
  #define LCD_PINS_RS                         27
  #define LCD_PINS_ENABLE                     29
  #define LCD_PINS_D4                         37
  #define LCD_PINS_D5                         35
  #define LCD_PINS_D6                         33
  #define LCD_PINS_D7                         31

  // Buttons
  #undef BTN_EN1
  #undef BTN_EN2
  #undef BTN_ENC
  #define BTN_EN1                             17
  #define BTN_EN2                             16
  #define BTN_ENC                             23

#else

  #define BEEPER_PIN                          33

#endif
