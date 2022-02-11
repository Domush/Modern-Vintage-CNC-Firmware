/**
 * Modern Vintage CNC Firmware
*/

#include "../../../inc/mvCNCConfig.h"

#if ENABLED(ADVANCED_PAUSE_FEATURE)

#include "../../gcode.h"
#include "../../../feature/pause.h"
#include "../../../module/motion.h"
#include "../../../module/jobcounter.h"

#if HAS_MULTI_EXTRUDER
  #include "../../../module/tool_change.h"
#endif

/**
 * M603: Configure filament change
 *
 *  T[toolhead] - Select extruder to configure, active extruder if not specified
 *  U[distance] - Retract distance for removal, for the specified extruder
 *  L[distance] - Extrude distance for insertion, for the specified extruder
 */
void GcodeSuite::M603() {

  if (!parser.seen("TUL")) return M603_report();

  const int8_t target_extruder = get_target_extruder_from_command();
  if (target_extruder < 0) return;

  // Unload length
  if (parser.seen('U')) {
    fc_settings[target_extruder].unload_length = ABS(parser.value_axis_units(E_AXIS));
    #if ENABLED(PREVENT_LENGTHY_EXTRUDE)
      NOMORE(fc_settings[target_extruder].unload_length, EXTRUDE_MAXLENGTH);
    #endif
  }

  // Load length
  if (parser.seen('L')) {
    fc_settings[target_extruder].load_length = ABS(parser.value_axis_units(E_AXIS));
    #if ENABLED(PREVENT_LENGTHY_EXTRUDE)
      NOMORE(fc_settings[target_extruder].load_length, EXTRUDE_MAXLENGTH);
    #endif
  }
}

void GcodeSuite::M603_report(const bool forReplay/*=true*/) {
  report_heading(forReplay, F(STR_FILAMENT_LOAD_UNLOAD));

  #if EXTRUDERS == 1
    report_echo_start(forReplay);
    SERIAL_ECHOPGM("  M603 L", LINEAR_UNIT(fc_settings[0].load_length), " U", LINEAR_UNIT(fc_settings[0].unload_length), " ;");
    say_units();
  #else
    LOOP_L_N(e, EXTRUDERS) {
      report_echo_start(forReplay);
      SERIAL_ECHOPGM("  M603 T", e, " L", LINEAR_UNIT(fc_settings[e].load_length), " U", LINEAR_UNIT(fc_settings[e].unload_length), " ;");
      say_units();
    }
  #endif
}

#endif // ADVANCED_PAUSE_FEATURE
