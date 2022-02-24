/**
 * Modern Vintage CNC Firmware
*/

#include "../../inc/mvCNCConfigPre.h"

#if ENABLED(CNC_ID_CHECK)

#include "../gcode.h"
#include "../../mvCNCCore.h"

/**
 * M16: Expected CNC Check
 */
void GcodeSuite::M16() {

  if (strcmp_P(parser.string_arg, PSTR(MACHINE_NAME)))
    kill(GET_TEXT_F(MSG_KILL_EXPECTED_PRINTER));

}

#endif
