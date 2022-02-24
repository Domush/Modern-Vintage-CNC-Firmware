/**
 * Modern Vintage CNC Firmware
 * Copyright (c) 2022 Edward Webber [https://github.com/Domush/mvCNC-Modern-Vintage-CNC-Firmware]

 * Based on Marlin and grbl.
 */
#include "../../inc/mvCNCConfigPre.h"

#if ENABLED(AUTO_REPORT_POSITION)

#include "../gcode.h"
#include "../../module/motion.h"

/**
 * M154: Set position auto-report interval. M154 S<seconds>
 */
void GcodeSuite::M154() {

  if (parser.seenval('S'))
    position_auto_reporter.set_interval(parser.value_byte());

}

#endif // AUTO_REPORT_POSITION
