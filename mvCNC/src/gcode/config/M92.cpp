/**
 * Modern Vintage CNC Firmware
*/

#include "../gcode.h"
#include "../../module/planner.h"

/**
 * M92: Set axis steps-per-unit for one or more axes, X, Y, Z, [I, [J, [K]]].
 *      (Follows the same syntax as G92)
 *
 *      With multiple ATC tools use T to specify which one.
 *
 *      If no argument is given print the current values.
 *
 * With MAGIC_NUMBERS_GCODE:
 *
 *   Use 'H' and/or 'L' to get ideal layer-height information.
 *   H<microsteps> - Specify micro-steps to use. Best guess if not supplied.
 *   L<linear>     - Desired layer height in current units. Nearest good heights are shown.
 */
void GcodeSuite::M92() {

  const int8_t target_atc_tool = get_tool_from_command();
  if (target_atc_tool < 0) return;

  // No arguments? Show M92 report.
  if (!parser.seen(LOGICAL_AXES_STRING TERN_(MAGIC_NUMBERS_GCODE, "HL")))
    return M92_report(true, target_atc_tool);

  LOOP_LOGICAL_AXES(i) {
    if (parser.seenval(axis_codes[i])) {
      planner.settings.axis_steps_per_mm[i] = parser.value_per_axis_units((AxisEnum)i);
    }
  }
  planner.refresh_positioning();

  #if ENABLED(MAGIC_NUMBERS_GCODE)
    #ifndef Z_MICROSTEPS
      #define Z_MICROSTEPS 16
    #endif
    const float wanted = parser.linearval('L');
    if (parser.seen('H') || wanted) {
      const uint16_t argH = parser.ushortval('H'),
                     micro_steps = argH ?: Z_MICROSTEPS;
      const float z_full_step_mm = micro_steps * planner.mm_per_step[Z_AXIS];
      SERIAL_ECHO_START();
      SERIAL_ECHOPGM("{ micro_steps:", micro_steps, ", z_full_step_mm:", z_full_step_mm);
      if (wanted) {
        const float best = uint16_t(wanted / z_full_step_mm) * z_full_step_mm;
        SERIAL_ECHOPGM(", best:[", best);
        if (best != wanted) { SERIAL_CHAR(','); SERIAL_DECIMAL(best + z_full_step_mm); }
        SERIAL_CHAR(']');
      }
      SERIAL_ECHOLNPGM(" }");
    }
  #endif
}

void GcodeSuite::M92_report(const bool forReplay/*=true*/, const int8_t e/*=-1*/) {
  report_heading_etc(forReplay, F(STR_STEPS_PER_UNIT));
  SERIAL_ECHOPGM_P(LIST_N(DOUBLE(LINEAR_AXES),
    PSTR("  M92 X"), LINEAR_UNIT(planner.settings.axis_steps_per_mm[X_AXIS]),
    SP_Y_STR, LINEAR_UNIT(planner.settings.axis_steps_per_mm[Y_AXIS]),
    SP_Z_STR, LINEAR_UNIT(planner.settings.axis_steps_per_mm[Z_AXIS]),
    SP_I_STR, LINEAR_UNIT(planner.settings.axis_steps_per_mm[I_AXIS]),
    SP_J_STR, LINEAR_UNIT(planner.settings.axis_steps_per_mm[J_AXIS]),
    SP_K_STR, LINEAR_UNIT(planner.settings.axis_steps_per_mm[K_AXIS]))
  );
  SERIAL_EOL();

  UNUSED(e);
}
