/**
 * Modern Vintage CNC Firmware
*/

#include "../gcode.h"
#include "../../module/planner.h"

#if HAS_ATC_TOOLS

/**
 * M221: Set extrusion percentage (M221 T0 S95)
 */
void GcodeSuite::M221() {

  const int8_t target_atc_tool = get_tool_from_command();
  if (target_atc_tool < 0) return;

  if (parser.seenval('S'))
    planner.set_flow(target_atc_tool, parser.value_int());
  else {
    SERIAL_ECHO_START();
    SERIAL_CHAR('E', '0' + target_atc_tool);
    SERIAL_ECHOPGM(" Flow: ", planner.flow_percentage[target_atc_tool]);
    SERIAL_CHAR('%');
    SERIAL_EOL();
  }
}

#endif // ATC_TOOLS
