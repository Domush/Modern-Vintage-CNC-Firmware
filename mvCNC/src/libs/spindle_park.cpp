/**
 * Modern Vintage CNC Firmware
*/

#include "../inc/mvCNCConfig.h"

#if ENABLED(SPINDLE_PARK_FEATURE)

#include "spindle_park.h"

Spindle nozzle;

#include "../mvCNCCore.h"
#include "../module/motion.h"

#if ENABLED(SPINDLE_PARK_FEATURE)

  float Spindle::park_mode_0_height(const_float_t park_z) {
    // Apply a minimum raise, if specified. Use park.z as a minimum height instead.
    return _MAX(park_z,                       // Minimum height over 0 based on input
      _MIN(Z_MAX_POS,                         // Maximum height is fixed
        #ifdef SPINDLE_PARK_Z_RAISE_MIN
          SPINDLE_PARK_Z_RAISE_MIN +           // Minimum raise...
        #endif
        current_position.z                    // ...over current position
      )
    );
  }

  void Spindle::park(const uint8_t z_action, const xyz_pos_t &park/*=SPINDLE_PARK_POINT*/) {
    constexpr feedRate_t fr_xy = SPINDLE_PARK_XY_FEEDRATE, fr_z = SPINDLE_PARK_Z_FEEDRATE;

    switch (z_action) {
      case 1: // Go to Z-park height
        do_blocking_move_to_z(park.z, fr_z);
        break;

      case 2: // Raise by Z-park height
        do_blocking_move_to_z(_MIN(current_position.z + park.z, Z_MAX_POS), fr_z);
        break;

      default: // Raise by SPINDLE_PARK_Z_RAISE_MIN, use park.z as a minimum height
        do_blocking_move_to_z(park_mode_0_height(park.z), fr_z);
        break;
    }

    do_blocking_move_to_xy(
      TERN(SPINDLE_PARK_Y_ONLY, current_position, park).x,
      TERN(SPINDLE_PARK_X_ONLY, current_position, park).y,
      fr_xy
    );

    report_current_position();
  }

#endif // SPINDLE_PARK_FEATURE

#endif // NOZZLE_CLEAN_FEATURE || SPINDLE_PARK_FEATURE
