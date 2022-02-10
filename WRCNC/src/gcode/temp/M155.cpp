/**
 * Webber Ranch CNC Firmware
*/

#include "../../inc/WRCNCConfig.h"

#if BOTH(AUTO_REPORT_TEMPERATURES, HAS_TEMP_SENSOR)

#include "../gcode.h"
#include "../../module/temperature.h"

/**
 * M155: Set temperature auto-report interval. M155 S<seconds>
 */
void GcodeSuite::M155() {

  if (parser.seenval('S'))
    thermalManager.auto_reporter.set_interval(parser.value_byte());

}

#endif // AUTO_REPORT_TEMPERATURES && HAS_TEMP_SENSOR
