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
#include "NintendoExtensionCtrl.h"

class WiiNunchuck {
  friend class Temperature;
  private:
    static temp_info_t x;
    static temp_info_t y;
    static temp_info_t z;
  public:
#if ENABLED(WII_NUNCHUCK_DEBUG)
   static void report();
#endif
   static void calculate(xyz_float_t &norm_jog);
   static void inject_jog_moves();
};

extern WiiNunchuck wiinunchuck;
