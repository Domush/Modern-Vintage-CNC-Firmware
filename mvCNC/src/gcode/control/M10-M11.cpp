/**
 * Modern Vintage CNC Firmware
 * Copyright (c) 2022 Edward Webber [https://github.com/Domush/mvCNC-Modern-Vintage-CNC-Firmware]

 * Based on Marlin and grbl.
 */
#include "../../inc/mvCNCConfig.h"

#if ENABLED(AIR_EVACUATION)

#include "../gcode.h"
#include "../../feature/spindle_laser.h"

/**
 * M10: Vacuum or Blower On
 */
void GcodeSuite::M10() {
  cutter.air_evac_enable();   // Turn on Vacuum or Blower motor
}

/**
 * M11: Vacuum or Blower OFF
 */
void GcodeSuite::M11() {
  cutter.air_evac_disable();  // Turn off Vacuum or Blower motor
}

#endif // AIR_EVACUATION
