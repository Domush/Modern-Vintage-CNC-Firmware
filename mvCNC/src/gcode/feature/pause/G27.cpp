/**
 * Modern Vintage CNC Firmware
*/

#include "../../../inc/mvCNCConfig.h"

#if ENABLED(SPINDLE_PARK_FEATURE)

#include "../../gcode.h"
#include "../../../libs/nozzle.h"
#include "../../../module/motion.h"

/**
 * G27: Park the nozzle
 */
void GcodeSuite::G27() {
  // Don't allow nozzle parking without homing first
  if (homing_needed_error()) return;
  nozzle.park(parser.ushortval('P'));
}

#endif // SPINDLE_PARK_FEATURE
