/**
 * Modern Vintage CNC Firmware
*/

#include "../../inc/mvCNCConfig.h"

#if ENABLED(HOST_PROMPT_SUPPORT) && DISABLED(EMERGENCY_PARSER)

#include "../../feature/host_actions.h"
#include "../gcode.h"
#include "../../mvCNCCore.h"

/**
 * M876: Handle Prompt Response
 */
void GcodeSuite::M876() {

  if (parser.seenval('S')) hostui.handle_response((uint8_t)parser.value_int());

}

#endif // HOST_PROMPT_SUPPORT && !EMERGENCY_PARSER
