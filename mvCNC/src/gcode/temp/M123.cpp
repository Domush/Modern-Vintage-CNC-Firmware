/**
 * Modern Vintage CNC Firmware
 * Copyright (c) 2022 Edward Webber [https://github.com/Domush/mvCNC-Modern-Vintage-CNC-Firmware]

 * Based on Marlin and grbl.
 */
#include "../../inc/mvCNCConfig.h"

#if HAS_FANCHECK

#include "../gcode.h"
#include "../../feature/fancheck.h"

/**
 * M123: Report fan states -or- set interval for auto-report
 *
 *   S<seconds> : Set auto-report interval
 */
void GcodeSuite::M123() {

  #if ENABLED(AUTO_REPORT_FANS)
    if (parser.seenval('S')) {
      fan_check.auto_reporter.set_interval(parser.value_byte());
      return;
    }
  #endif

  fan_check.print_fan_states();

}

#endif // HAS_FANCHECK
