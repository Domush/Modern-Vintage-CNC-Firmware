/**
 * Modern Vintage CNC Firmware
*/
#pragma once

/**
 * joystick.h - joystick input / jogging
 */

#include "../inc/mvCNCConfigPre.h"
#include "../core/types.h"
#include "../module/temperature.h"

class Joystick {
  friend class Temperature;
  private:
    #if HAS_JOY_ADC_X
      static temp_info_t x;
    #endif
    #if HAS_JOY_ADC_Y
      static temp_info_t y;
    #endif
    #if HAS_JOY_ADC_Z
      static temp_info_t z;
    #endif
  public:
    #if ENABLED(JOYSTICK_DEBUG)
      static void report();
    #endif
    static void calculate(xyz_float_t &norm_jog);
    static void injectJogMoves();
};

extern Joystick joystick;
