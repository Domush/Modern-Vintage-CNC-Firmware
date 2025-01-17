/**
 * Modern Vintage CNC Firmware
*/

#include "../../inc/mvCNCConfig.h"

#if HAS_M206_COMMAND

#include "../gcode.h"
#include "../../module/motion.h"
#include "../../lcd/mvcncui.h"
#include "../../libs/buzzer.h"
#include "../../mvCNCCore.h"

/**
 * M206: Set Additional Homing Offset (X Y Z). SCARA aliases T=X, P=Y
 *
 * *** @thinkyhead: I recommend deprecating M206 for SCARA in favor of M665.
 * ***              M206 for SCARA will remain enabled in 1.1.x for compatibility.
 * ***              In the 2.0 release, it will simply be disabled by default.
 */
void GcodeSuite::M206() {
  if (!parser.seen_any()) return M206_report();

  LOOP_LINEAR_AXES(i)
    if (parser.seen(AXIS_CHAR(i)))
      set_home_offset((AxisEnum)i, parser.value_linear_units());

  #if ENABLED(MORGAN_SCARA)
    if (parser.seen('T')) set_home_offset(A_AXIS, parser.value_float()); // Theta
    if (parser.seen('P')) set_home_offset(B_AXIS, parser.value_float()); // Psi
  #endif

  report_current_position();
}

void GcodeSuite::M206_report(const bool forReplay/*=true*/) {
  report_heading_etc(forReplay, F(STR_HOME_OFFSET));
  SERIAL_ECHOLNPGM_P(
    #if IS_CARTESIAN
      LIST_N(DOUBLE(LINEAR_AXES),
        PSTR("  M206 X"), LINEAR_UNIT(home_offset.x),
        SP_Y_STR, LINEAR_UNIT(home_offset.y),
        SP_Z_STR, LINEAR_UNIT(home_offset.z),
        SP_I_STR, LINEAR_UNIT(home_offset.i),
        SP_J_STR, LINEAR_UNIT(home_offset.j),
        SP_K_STR, LINEAR_UNIT(home_offset.k)
      )
    #else
      PSTR("  M206 Z"), LINEAR_UNIT(home_offset.z)
    #endif
  );
}

/**
 * M428: Set home_offset based on the distance between the
 *       current_position and the nearest "reference point."
 *       If an axis is past center its endstop position
 *       is the reference-point. Otherwise it uses 0. This allows
 *       the Z offset to be set near the bed when using a max endstop.
 *
 *       M428 can't be used more than 2cm away from 0 or an endstop.
 *
 *       Use M206 to set these values directly.
 */
void GcodeSuite::M428() {
  if (homing_needed_error()) return;

  xyz_float_t diff;
  LOOP_LINEAR_AXES(i) {
    diff[i] = base_home_pos((AxisEnum)i) - current_position[i];
    if (!WITHIN(diff[i], -20, 20) && home_dir((AxisEnum)i) > 0)
      diff[i] = -current_position[i];
    if (!WITHIN(diff[i], -20, 20)) {
      SERIAL_ERROR_MSG(STR_ERR_M428_TOO_FAR);
      LCD_ALERTMESSAGE_F("Err: Too far!");
      BUZZ(200, 40);
      return;
    }
  }

  LOOP_LINEAR_AXES(i) set_home_offset((AxisEnum)i, diff[i]);
  report_current_position();
  LCD_MESSAGE(MSG_HOME_OFFSETS_APPLIED);
  BUZZ(100, 659);
  BUZZ(100, 698);
}

#endif // HAS_M206_COMMAND
