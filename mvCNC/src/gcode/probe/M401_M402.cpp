/**
 * Modern Vintage CNC Firmware
*/

#include "../../inc/mvCNCConfig.h"

#if HAS_BED_PROBE

#include "../gcode.h"
#include "../../module/motion.h"
#include "../../module/probe.h"

/**
 * M401: Deploy and activate the Z probe
 *
 * With BLTOUCH_HS_MODE:
 *  S<bool> Set High Speed (HS) Mode and exit without deploy
 */
void GcodeSuite::M401() {
  if (parser.seen('S')) {
    #ifdef BLTOUCH_HS_MODE
      bltouch.high_speed_mode = parser.value_bool();
    #endif
  }
  else {
    probe.deploy();
    TERN_(PROBE_TARE, probe.tare());
    report_current_position();
  }
}

/**
 * M402: Deactivate and stow the Z probe
 */
void GcodeSuite::M402() {
  probe.stow();
  probe.move_z_after_probing();
  report_current_position();
}

#endif // HAS_BED_PROBE
