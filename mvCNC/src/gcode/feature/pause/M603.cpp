/**
 * Modern Vintage CNC Firmware
*/

#include "../../../inc/mvCNCConfig.h"

#if ENABLED(ADVANCED_PAUSE_FEATURE)

#include "../../gcode.h"
#include "../../../feature/pause.h"
#include "../../../module/motion.h"
#include "../../../module/jobcounter.h"

#if TOOL_CHANGE_SUPPORT
  #include "../../../module/tool_change.h"
#endif

/**
 * M603: Configure filament change
 *
 *  T[toolhead] - Select ATC tool to configure, active ATC tool if not specified
 *  U[distance] - Retract distance for removal, for the specified ATC tool
 *  L[distance] - Extrude distance for insertion, for the specified ATC tool
 */
void GcodeSuite::M603() {

  if (!parser.seen("TUL")) return M603_report();

  const int8_t target_atc_tool = get_tool_from_command();
  if (target_atc_tool < 0) return;

  // Unload length
  if (parser.seen('U')) {
    fc_settings[target_atc_tool].unload_length = ABS(parser.value_axis_units(E_AXIS));
    #if ENABLED(PREVENT_LENGTHY_EXTRUDE)
      NOMORE(fc_settings[target_atc_tool].unload_length, EXTRUDE_MAXLENGTH);
    #endif
  }

  // Load length
  if (parser.seen('L')) {
    fc_settings[target_atc_tool].load_length = ABS(parser.value_axis_units(E_AXIS));
    #if ENABLED(PREVENT_LENGTHY_EXTRUDE)
      NOMORE(fc_settings[target_atc_tool].load_length, EXTRUDE_MAXLENGTH);
    #endif
  }
}

void GcodeSuite::M603_report(const bool forReplay/*=true*/) {
  report_heading(forReplay, F(STR_FILAMENT_LOAD_UNLOAD));

  #if ATC_TOOLS == 1
    report_echo_start(forReplay);
    SERIAL_ECHOPGM("  M603 L", LINEAR_UNIT(fc_settings[0].load_length), " U", LINEAR_UNIT(fc_settings[0].unload_length), " ;");
    say_units();
  #else
    LOOP_L_N(e, ATC_TOOLS) {
      report_echo_start(forReplay);
      SERIAL_ECHOPGM("  M603 T", e, " L", LINEAR_UNIT(fc_settings[e].load_length), " U", LINEAR_UNIT(fc_settings[e].unload_length), " ;");
      say_units();
    }
  #endif
}

#endif // ADVANCED_PAUSE_FEATURE
