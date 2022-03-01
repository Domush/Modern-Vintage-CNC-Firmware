/**
 * Modern Vintage CNC Firmware
*/

#include "../gcode.h"
#include "../../module/fan_control.h"

/**
 * M105: Report temps (kept for compatibility's sake)
 */
void GcodeSuite::M105() {

  SERIAL_ECHOPGM(STR_OK);
  SERIAL_ECHOLNPGM(" T:0"); // Some hosts send M105 to test the serial connection
}
