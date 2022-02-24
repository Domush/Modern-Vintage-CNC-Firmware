/**
 * Modern Vintage CNC Firmware
 * Copyright (c) 2022 Edward Webber [https://github.com/Domush/mvCNC-Modern-Vintage-CNC-Firmware]

 * Based on Marlin and grbl.
 */
/**
 * gcode/temp/M143_M193.cpp
 *
 * Laser Cooler target temperature control
 */

#include "../../inc/mvCNCConfig.h"

#if HAS_COOLER

#include "../../feature/cooler.h"
extern Cooler cooler;

#include "../gcode.h"
#include "../../module/temperature.h"
#include "../../lcd/mvcncui.h"

/**
 * M143: Set cooler temperature
 */
void GcodeSuite::M143() {
  if (DEBUGGING(DRYRUN)) return;
  if (parser.seenval('S')) {
    thermalManager.setTargetCooler(parser.value_celsius());
    parser.value_celsius() ? cooler.enable() : cooler.disable();
  }
}

/**
 * M193: Sxxx Wait for laser current temp to reach target temp. Waits only when cooling.
 */
void GcodeSuite::M193() {
  if (DEBUGGING(DRYRUN)) return;

  if (parser.seenval('S')) {
    cooler.enable();
    thermalManager.setTargetCooler(parser.value_celsius());
    if (thermalManager.isLaserCooling()) {
      LCD_MESSAGE(MSG_LASER_COOLING);
      thermalManager.wait_for_cooler(true);
    }
  }
}

#endif // HAS_COOLER
