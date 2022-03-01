/**
 * Modern Vintage CNC Firmware
*/

#include "../../inc/mvCNCConfig.h"

#if EITHER(EXT_SOLENOID, MANUAL_SOLENOID_CONTROL)

#include "../gcode.h"
#include "../../feature/solenoid.h"
#include "../../module/motion.h"

/**
 * M380: Enable solenoid on the active ATC tool
 *
 *   S<index> to specify a solenoid (Requires MANUAL_SOLENOID_CONTROL)
 */
void GcodeSuite::M380() {
  #if ENABLED(MANUAL_SOLENOID_CONTROL)
    enable_solenoid(parser.intval('S', active_tool));
  #else
    enable_solenoid_on_active_tool();
  #endif
}

/**
 * M381: Disable all solenoids if EXT_SOLENOID
 *       Disable selected/active solenoid if MANUAL_SOLENOID_CONTROL
 */
void GcodeSuite::M381() {
  #if ENABLED(MANUAL_SOLENOID_CONTROL)
    disable_solenoid(parser.intval('S', active_tool));
  #else
    disable_all_solenoids();
  #endif
}

#endif // EXT_SOLENOID || MANUAL_SOLENOID_CONTROL
