/**
 * Modern Vintage CNC Firmware
*/

#include "../gcode.h"
#include "../../module/tool_change.h"

#if EITHER(TOOL_CHANGE_SUPPORT, DEBUG_LEVELING_FEATURE)
  #include "../../module/motion.h"
#endif

#if HAS_PRUSA_MMU2
  #include "../../feature/mmu/mmu2.h"
#endif

#define DEBUG_OUT ENABLED(DEBUG_LEVELING_FEATURE)
#include "../../core/debug_out.h"

/**
 * T0-T<n>: Switch tool, usually switching extruders
 *
 *   F[units/min] Set the movement feedrate
 *   S1           Don't move the tool in XY after change
 *
 * For PRUSA_MMU2(S) and EXTENDABLE_EMU_MMU2(S)
 *   T[n] Gcode to extrude at least 38.10 mm at feedrate 19.02 mm/s must follow immediately to load to extruder wheels.
 *   T?   Gcode to extrude shouldn't have to follow. Load to extruder wheels is done automatically.
 *   Tx   Same as T?, but nozzle doesn't have to be preheated. Tc requires a preheated nozzle to finish filament load.
 *   Tc   Load to nozzle after filament was prepared by Tc and nozzle is already heated.
 */
void GcodeSuite::T(const int8_t tool_index) {

  DEBUG_SECTION(log_T, "T", DEBUGGING(LEVELING));
  if (DEBUGGING(LEVELING)) DEBUG_ECHOLNPGM("...(", tool_index, ")");

  // Count this command as movement / activity
  reset_stepper_timeout();

  #if HAS_PRUSA_MMU2
    if (parser.string_arg) {
      mmu2.tool_change(parser.string_arg);   // Special commands T?/Tx/Tc
      return;
    }
  #endif

  tool_change(tool_index
    #if TOOL_CHANGE_SUPPORT
      ,  TERN(PARKING_EXTRUDER, false, tool_index == active_extruder) // For PARKING_EXTRUDER motion is decided in tool_change()
      || parser.boolval('S')
    #endif
  );
}
