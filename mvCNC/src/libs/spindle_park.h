/**
 * Modern Vintage CNC Firmware
*/
#pragma once

#include "../inc/mvCNCConfig.h"

/**
 * @brief Spindle class
 *
 * @todo: Do not ignore the end.z value and allow XYZ movements
 */
class Spindle {
  private:

  public:

  #if ENABLED(SPINDLE_PARK_FEATURE)

    static float park_mode_0_height(const_float_t park_z) _Os;
    static void park(const uint8_t z_action, const xyz_pos_t &park=SPINDLE_PARK_POINT) _Os;

  #endif
};

extern Spindle tool;
