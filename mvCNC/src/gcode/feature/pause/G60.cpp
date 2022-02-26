/**
 * Modern Vintage CNC Firmware
*/

#include "../../../inc/mvCNCConfig.h"

#if SAVED_POSITIONS

#include "../../gcode.h"
#include "../../../module/motion.h"

#define DEBUG_OUT ENABLED(SAVED_POSITIONS_DEBUG)
#include "../../../core/debug_out.h"

/**
 * G60: Save current position
 *
 *   S<slot> - Memory slot # (0-based) to save into (default 0)
 */
void GcodeSuite::G60() {
  const uint8_t slot = parser.byteval('S');

  if (slot >= SAVED_POSITIONS) {
    SERIAL_ERROR_MSG(STR_INVALID_POS_SLOT STRINGIFY(SAVED_POSITIONS));
    return;
  }

  stored_position[slot] = current_position;
  SBI(saved_slots[slot >> 3], slot & 0x07);

  #if ENABLED(SAVED_POSITIONS_DEBUG)
  {
    DEBUG_ECHOPGM(STR_SAVED_POS " S", slot);
    const xyze_pos_t &pos = stored_position[slot];
    DEBUG_ECHOLNPAIR_F_P(LIST_N(DOUBLE(LINEAR_AXES), PSTR(" : X"), pos.x, SP_Y_STR, pos.y, SP_Z_STR, pos.z, SP_I_STR,
                                pos.i, SP_J_STR, pos.j, SP_K_STR, pos.k));
  }
  #endif
}

#endif // SAVED_POSITIONS
