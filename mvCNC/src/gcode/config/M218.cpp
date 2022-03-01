/**
 * Modern Vintage CNC Firmware
*/

#include "../../inc/mvCNCConfig.h"

#if HAS_HOTEND_OFFSET

#include "../gcode.h"
#include "../../module/motion.h"

#if ENABLED(DELTA)
  #include "../../module/planner.h"
#endif

/**
 * M218 - set hotend offset (in linear units)
 *
 *   T<tool>
 *   X<xoffset>
 *   Y<yoffset>
 *   Z<zoffset>
 */
void GcodeSuite::M218() {

  if (!parser.seen_any()) return M218_report();

  const int8_t target_atc_tool = get_tool_from_command();
  if (target_atc_tool < 0) return;

  if (parser.seenval('X')) hotend_offset[target_atc_tool].x = parser.value_linear_units();
  if (parser.seenval('Y')) hotend_offset[target_atc_tool].y = parser.value_linear_units();
  if (parser.seenval('Z')) hotend_offset[target_atc_tool].z = parser.value_linear_units();

  #if ENABLED(DELTA)
    if (target_atc_tool == active_atc_tool)
      do_blocking_move_to_xy(current_position, planner.settings.max_feedrate_mm_s[X_AXIS]);
  #endif
}

void GcodeSuite::M218_report(const bool forReplay/*=true*/) {
  report_heading_etc(forReplay, F(STR_HOTEND_OFFSETS));
  LOOP_S_L_N(e, 1, HOTENDS) {
    report_echo_start(forReplay);
    SERIAL_ECHOPGM_P(
      PSTR("  M218 T"), e,
      SP_X_STR, LINEAR_UNIT(hotend_offset[e].x),
      SP_Y_STR, LINEAR_UNIT(hotend_offset[e].y)
    );
    SERIAL_ECHOLNPAIR_F_P(SP_Z_STR, LINEAR_UNIT(hotend_offset[e].z), 3);
  }
}

#endif // HAS_HOTEND_OFFSET
