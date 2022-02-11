/**
 * Modern Vintage CNC Firmware
*/

/**
 * gcode/temp/M140_M190.cpp
 *
 * Bed target temperature control
 */

#include "../../inc/mvCNCConfig.h"

#if HAS_HEATED_BED

#include "../gcode.h"
#include "../../module/temperature.h"
#include "../../lcd/mvcncui.h"

/**
 * M140 - Set Bed Temperature target and return immediately
 * M190 - Set Bed Temperature target and wait
 *
 *  I<index>  : Preset index (if material presets are defined)
 *  S<target> : The target temperature in current units
 *
 * Parameters
 *  I<index>  : Preset index (if material presets are defined)
 *  S<target> : The target temperature in current units. Wait for heating only.
 *
 * M190 Parameters
 *  R<target> : The target temperature in current units. Wait for heating and cooling.
 *
 * Examples
 *  M140 S60 : Set target to 60° and return right away.
 *  M190 R40 : Set target to 40°. Wait until the bed gets close to 40°.
 *
 * With JOB_TIMER_AUTOSTART turning on heaters will start the CNC job timer
 *  (used by printingIsActive, etc.) and turning off heaters will stop the timer.
 */
void GcodeSuite::M140_M190(const bool isM190) {

  if (DEBUGGING(DRYRUN)) return;

  bool got_temp = false;
  celsius_t temp = 0;

  // Accept 'I' if temperature presets are defined
  #if HAS_PREHEAT
    got_temp = parser.seenval('I');
    if (got_temp) {
      const uint8_t index = parser.value_byte();
      temp = ui.material_preset[_MIN(index, PREHEAT_COUNT - 1)].bed_temp;
    }
  #endif

  // Get the temperature from 'S' or 'R'
  bool no_wait_for_cooling = false;
  if (!got_temp) {
    no_wait_for_cooling = parser.seenval('S');
    got_temp = no_wait_for_cooling || (isM190 && parser.seenval('R'));
    if (got_temp) temp = parser.value_celsius();
  }

  if (!got_temp) return;

  thermalManager.setTargetBed(temp);

  ui.set_status(thermalManager.isHeatingBed() ? GET_TEXT_F(MSG_BED_HEATING) : GET_TEXT_F(MSG_BED_COOLING));

  // with JOB_TIMER_AUTOSTART, M190 can start the timer, and M140 can stop it
  TERN_(JOB_TIMER_AUTOSTART, thermalManager.auto_job_check_timer(isM190, !isM190));

  if (isM190)
    thermalManager.wait_for_bed(no_wait_for_cooling);
}

#endif // HAS_HEATED_BED
