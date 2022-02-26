/**
 * Modern Vintage CNC Firmware
*/

#include "../../../inc/mvCNCConfig.h"

#if SAVED_POSITIONS

#include "../../../module/planner.h"
#include "../../gcode.h"
#include "../../../module/motion.h"
#include "../../../module/planner.h"

#define DEBUG_OUT ENABLED(SAVED_POSITIONS_DEBUG)
#include "../../../core/debug_out.h"

/**
 * G61: Return to saved position
 *
 *   F<rate>  - Feedrate (optional) for the move back.
 *   S<slot>  - Slot # (0-based) to restore from (default 0).
 *   X Y Z E  - Axes to restore. At least one is required.
 *
 *   If XYZE are not given, default restore uses the smart blocking move.
 */
void GcodeSuite::G61() {

  const uint8_t slot = parser.byteval('S');

  #if SAVED_POSITIONS < 256
    if (slot >= SAVED_POSITIONS) {
      SERIAL_ERROR_MSG(STR_INVALID_POS_SLOT STRINGIFY(SAVED_POSITIONS));
      return;
    }
  #endif

  // No saved position? No axes being restored?
  if (!TEST(saved_slots[slot >> 3], slot & 0x07)) return;

  // Apply any given feedrate over 0.0
  feedRate_t saved_feedrate = feedrate_mm_s;
  const float fr = parser.linearval('F');
  if (fr > 0.0) feedrate_mm_s = MMM_TO_MMS(fr);

  if (!parser.seen_axis()) {
    DEBUG_ECHOLNPGM("Default position restore");
    do_blocking_move_to(stored_position[slot], feedrate_mm_s);
  }
  else {
    if (parser.seen(LINEAR_AXIS_GANG("X", "Y", "Z", STR_I, STR_J, STR_K))) {
      DEBUG_ECHOPGM(STR_RESTORING_POS " S", slot);
      LOOP_LINEAR_AXES(i) {
        destination[i] = parser.seen(AXIS_CHAR(i))
          ? stored_position[slot][i] + parser.value_axis_units((AxisEnum)i)
          : current_position[i];
        DEBUG_CHAR(' ', AXIS_CHAR(i));
        DEBUG_ECHO_F(destination[i]);
      }
      DEBUG_EOL();
      // Move to the saved position
      prepare_line_to_destination();
    }
  }

  feedrate_mm_s = saved_feedrate;
}

#endif // SAVED_POSITIONS
