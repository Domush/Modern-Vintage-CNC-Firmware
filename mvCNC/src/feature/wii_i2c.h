/**
 * Modern Vintage CNC Firmware
*/
#pragma once

/**
 * joystick.h - joystick input / jogging
 */

#include "../inc/mvCNCConfigPre.h"
#include "../core/types.h"
#include "../../lib/wii_i2c/src/NintendoExtensionCtrl.h"

class WiiNunchuck {
  public:
    #if ENABLED(WII_NUNCHUCK_DEBUG)
      static void report();
    #endif
    static void calculate(xyz_float_t &norm_jog);
    static void inject_jog_moves();

   Nunchuk state;
   static void connect();
   static void update();
};

extern WiiNunchuck wii;
