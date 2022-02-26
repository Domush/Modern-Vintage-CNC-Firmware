/**
 * Modern Vintage CNC Firmware
*/
#pragma once

/**
 * feature/spindle_laser_types.h
 * Support for Laser Power or Spindle Power & Direction
 */

#include "../inc/mvCNCConfigPre.h"

#define MSG_CUTTER(M) _MSG_CUTTER(M)

#ifndef SPEED_POWER_INTERCEPT
  #define SPEED_POWER_INTERCEPT 0
#endif
#if ENABLED(SPINDLE_FEATURE)
  #define _MSG_CUTTER(M) MSG_SPINDLE_##M
  #ifndef SPEED_POWER_MIN
    #define SPEED_POWER_MIN      5000
  #endif
  #ifndef SPEED_POWER_MAX
    #define SPEED_POWER_MAX     30000
  #endif
  #ifndef SPEED_POWER_STARTUP
    #define SPEED_POWER_STARTUP 25000
  #endif
#else
  #define _MSG_CUTTER(M) MSG_LASER_##M
  #ifndef SPEED_POWER_MIN
    #define SPEED_POWER_MIN         0
  #endif
  #ifndef SPEED_POWER_MAX
    #define SPEED_POWER_MAX       255
  #endif
  #ifndef SPEED_POWER_STARTUP
    #define SPEED_POWER_STARTUP   255
  #endif
#endif

typedef IF<(SPEED_POWER_MAX > 255), uint16_t, uint8_t>::type cutter_cpower_t;

#if CUTTER_UNIT_IS(RPM) && SPEED_POWER_MAX > 255
  typedef uint16_t cutter_power_t;
  #define CUTTER_MENU_POWER_TYPE   uint16_5
  #define cutter_power2str         ui16tostr5rj
#else
  typedef uint8_t cutter_power_t;
  #if CUTTER_UNIT_IS(PERCENT)
    #define CUTTER_MENU_POWER_TYPE percent_3
    #define cutter_power2str       pcttostrpctrj
  #else
    #define CUTTER_MENU_POWER_TYPE uint8
    #define cutter_power2str       ui8tostr3rj
  #endif
#endif

#if ENABLED(LASER_FEATURE)
  typedef uint16_t cutter_test_pulse_t;
  #define CUTTER_MENU_PULSE_TYPE uint16_3
#endif

#if ENABLED(MVCNC_DEV_MODE)
  typedef uint16_t cutter_frequency_t;
  #define CUTTER_MENU_FREQUENCY_TYPE uint16_5
#endif
