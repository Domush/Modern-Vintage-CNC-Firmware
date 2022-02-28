/**
 * Modern Vintage CNC Firmware
*/

#include "utility.h"

#include "../mvCNCCore.h"
#include "../module/fan_control.h"

// A delay to provide brittle hosts time to receive bytes
#if ENABLED(SERIAL_OVERRUN_PROTECTION)

  #include "../gcode/gcode.h" // for set_autoreport_paused

  void serial_delay(const millis_t ms) {
    const bool was = gcode.set_autoreport_paused(true);
    safe_delay(ms);
    gcode.set_autoreport_paused(was);
  }
#endif
