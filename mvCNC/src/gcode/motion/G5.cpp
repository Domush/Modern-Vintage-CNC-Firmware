/**
 * Modern Vintage CNC Firmware
*/

#include "../../inc/mvCNCConfig.h"

#if ENABLED(BEZIER_CURVE_SUPPORT)

#if AXIS_COLLISION('I') || AXIS_COLLISION('J')
  #error "G5 parameter collision with axis name."
#endif

#include "../../module/motion.h"
#include "../../module/planner_bezier.h"

/**
 * Parameters interpreted according to:
 * https://linuxcnc.org/docs/2.7/html/gcode/g-code.html#gcode:g5
 * However I, J omission is not supported at this point; all
 * parameters can be omitted and default to zero.
 */

#include "../gcode.h"
#include "../../mvCNCCore.h" // for IsRunning()

/**
 * G5: Cubic B-spline
 */
void GcodeSuite::G5() {
  if (MOTION_CONDITIONS) {

    #if ENABLED(CNC_WORKSPACE_PLANES)
      if (workspace_plane != PLANE_XY) {
        SERIAL_ERROR_MSG(STR_ERR_BAD_PLANE_MODE);
        return;
      }
    #endif

    get_destination_from_command();

    const xy_pos_t offsets[2] = {
      { parser.linearval('I'), parser.linearval('J') },
      { parser.linearval('P'), parser.linearval('Q') }
    };

    cubic_b_spline(current_position, destination, offsets, MMS_SCALED(feedrate_mm_s), active_tool);
    current_position = destination;
  }
}

#endif // BEZIER_CURVE_SUPPORT
