/**
 * Modern Vintage CNC Firmware
*/
#pragma once

#if TOOL_CHANGE_SUPPORT || E_STEPPERS > 1
  #error "Ender-4 only supports one hotend / E-stepper. Comment out this line to continue."
#endif

#define BOARD_INFO_NAME "Ender-4"

#include "pins_RAMPS.h"

// The board only has one PWM fan connector. The others are 12V always-on.
// The default config uses this pin to control the brightness of the LED
// band (case light). Thus the hotend and controller fans are always-on.

#if ENABLED(CASE_LIGHT_ENABLE)
  #undef FAN_PIN
  #ifndef CASE_LIGHT_PIN
    #define CASE_LIGHT_PIN MOSFET_B_PIN
  #endif
#endif
