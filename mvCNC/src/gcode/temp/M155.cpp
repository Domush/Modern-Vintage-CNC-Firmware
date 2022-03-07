/**
 * Modern Vintage CNC Firmware
*/

#include "../../inc/mvCNCConfig.h"

#if BOTH(AUTO_REPORT_TEMPERATURES, HAS_TEMP_SENSOR)

#include "../gcode.h"
  #include "../../module/pwm_temp_io.h"

/**
 * M155: Set temperature auto-report interval. M155 S<seconds>
 */
void GcodeSuite::M155() {

  if (parser.seenval('S'))
    fanManager.auto_reporter.set_interval(parser.value_byte());

}

#endif // AUTO_REPORT_TEMPERATURES && HAS_TEMP_SENSOR
