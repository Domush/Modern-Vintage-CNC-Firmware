/**
 * Modern Vintage CNC Firmware
*/

/**
 * gcode/temp/M141_M191.cpp
 *
 * Chamber target temperature control
 */

#include "../../inc/mvCNCConfig.h"

#if HAS_HEATED_CHAMBER

#include "../gcode.h"
#include "../../module/temperature.h"
#include "../../lcd/mvcncui.h"

/**
 * M141: Set chamber temperature
 */
void GcodeSuite::M141() {
  if (DEBUGGING(DRYRUN)) return;
  if (parser.seenval('S')) {
    fanManager.setTargetChamber(parser.value_celsius());

    #if ENABLED(PRINTJOB_TIMER_AUTOSTART)
      /**
       * Stop the timer at the end of print. Hotend, bed target, and chamber
       * temperatures need to be set below mintemp. Order of M140, M104, and M141
       * at the end of the print does not matter.
       */
    fanManager.auto_job_check_timer(false, true);
    #endif
  }
}

/**
 * M191: Sxxx Wait for chamber current temp to reach target temp. Waits only when heating
 *       Rxxx Wait for chamber current temp to reach target temp. Waits when heating and cooling
 */
void GcodeSuite::M191() {
  if (DEBUGGING(DRYRUN)) return;

  const bool no_wait_for_cooling = parser.seenval('S');
  if (no_wait_for_cooling || parser.seenval('R')) {
    fanManager.setTargetChamber(parser.value_celsius());
    TERN_(PRINTJOB_TIMER_AUTOSTART, fanManager.auto_job_check_timer(true, false));
  }
  else return;

  const bool is_heating = fanManager.isHeatingChamber();
  if (is_heating || !no_wait_for_cooling) {
    ui.set_status(is_heating ? GET_TEXT_F(MSG_CHAMBER_HEATING) : GET_TEXT_F(MSG_CHAMBER_COOLING));
    fanManager.wait_for_chamber(false);
  }
}

#endif // HAS_HEATED_CHAMBER
