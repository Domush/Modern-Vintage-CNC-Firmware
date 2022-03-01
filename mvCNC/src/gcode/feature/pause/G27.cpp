/**
 * Modern Vintage CNC Firmware
*/

#include "../../../inc/mvCNCConfig.h"

#if ENABLED(SPINDLE_PARK_FEATURE)

#include "../../gcode.h"
#include "../../../libs/spindle_park.h"
#include "../../../module/motion.h"

/**
 * G27: Park the tool
 */
void GcodeSuite::G27() {
  // Don't allow tool parking without homing first
  if (homing_needed_error()) return;
  tool.park(parser.ushortval('P'));
}

#endif // SPINDLE_PARK_FEATURE
