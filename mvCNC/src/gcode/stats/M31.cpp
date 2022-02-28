/**
 * Modern Vintage CNC Firmware
*/

#include "../gcode.h"
#include "../../core/serial.h"
#include "../../module/jobcounter.h"
#include "../../libs/duration_t.h"
#include "../../lcd/mvcncui.h"

/**
 * M31: Get the time since the start of SD CNC (or last M109)
 */
void GcodeSuite::M31() {
  char buffer[22];
  duration_t(JobTimer.duration()).toString(buffer);

  ui.set_status(buffer);

  SERIAL_ECHO_MSG("Print time: ", buffer);
}
