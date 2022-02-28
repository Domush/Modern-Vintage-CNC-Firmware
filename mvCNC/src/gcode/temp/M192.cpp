/**
 * Modern Vintage CNC Firmware
 * Copyright (c) 2022 Edward Webber [https://github.com/Domush/mvCNC-Modern-Vintage-CNC-Firmware]

 * Based on Marlin and grbl.
 */
/**
 * M192.cpp - Wait for probe to reach temperature
 */

#include "../../inc/mvCNCConfig.h"

#if HAS_TEMP_PROBE

#include "../gcode.h"
#include "../../module/fan_control.h"
#include "../../lcd/mvcncui.h"

/**
 * M192: Wait for probe temperature sensor to reach a target
 *
 * Select only one of these flags:
 *    R - Wait for heating or cooling
 *    S - Wait only for heating
 */
void GcodeSuite::M192() {
  if (DEBUGGING(DRYRUN)) return;

  const bool no_wait_for_cooling = parser.seenval('S');
  if (!no_wait_for_cooling && !parser.seenval('R')) {
    SERIAL_ERROR_MSG("No target temperature set.");
    return;
  }

  const celsius_t target_temp = parser.value_celsius();
  ui.set_status(fanManager.isProbeBelowTemp(target_temp) ? GET_TEXT_F(MSG_PROBE_HEATING) : GET_TEXT_F(MSG_PROBE_COOLING));
  fanManager.wait_for_probe(target_temp, no_wait_for_cooling);
}

#endif // HAS_TEMP_PROBE
