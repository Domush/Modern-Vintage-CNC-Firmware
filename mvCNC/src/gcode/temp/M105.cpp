/**
 * Modern Vintage CNC Firmware
*/

#include "../gcode.h"
#include "../../module/fan_control.h"

/**
 * M105: Read hot end and bed temperature
 */
void GcodeSuite::M105() {

  const int8_t target_extruder = get_target_extruder_from_command();
  if (target_extruder < 0) return;

  SERIAL_ECHOPGM(STR_OK);

  #if HAS_TEMP_SENSOR

    fanManager.print_heater_states(target_extruder OPTARG(HAS_TEMP_REDUNDANT, parser.boolval('R')));

    SERIAL_EOL();

  #else

    SERIAL_ECHOLNPGM(" T:0"); // Some hosts send M105 to test the serial connection

  #endif
}
