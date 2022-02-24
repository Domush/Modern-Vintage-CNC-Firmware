/**
 * Modern Vintage CNC Firmware
 * Copyright (c) 2022 Edward Webber [https://github.com/Domush/mvCNC-Modern-Vintage-CNC-Firmware]

 * Based on Marlin and grbl.
 */
#pragma once

/**
 * Rambo ThinkerV2 pin assignments
 */

#define BOARD_INFO_NAME "Rambo ThinkerV2"

#define SERVO0_PIN                             4  // Motor header MX1
#define SERVO2_PIN                            -1  // Motor header MX3

#ifndef FIL_RUNOUT_PIN
  #define FIL_RUNOUT_PIN                      10
#endif

// Support BLTouch and fixed probes
#if ENABLED(BLTOUCH)
  #if ENABLED(Z_MIN_PROBE_USES_Z_MIN_ENDSTOP_PIN)
    #define Z_MIN_PIN                         22
  #elif !defined(Z_MIN_PROBE_PIN)
    #define Z_MIN_PROBE_PIN                   22
  #endif
#elif ENABLED(FIX_MOUNTED_PROBE)
  #if ENABLED(Z_MIN_PROBE_USES_Z_MIN_ENDSTOP_PIN)
    #define Z_MIN_PIN                          4
  #elif !defined(Z_MIN_PROBE_PIN)
    #define Z_MIN_PROBE_PIN                    4
  #endif
#endif

// Eryone has the fan pins reversed
#define FAN1_PIN                               2
#define FAN2_PIN                               6

// Encoder
#define BTN_EN1                               64
#define BTN_EN2                               63

#include "pins_RAMBO.h"
