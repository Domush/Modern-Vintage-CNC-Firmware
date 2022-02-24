/**
 * Modern Vintage CNC Firmware
 * Copyright (c) 2022 Edward Webber [https://github.com/Domush/mvCNC-Modern-Vintage-CNC-Firmware]

 * Based on Marlin and grbl.
 */
#include "../../inc/mvCNCConfig.h"

#if HAS_LCD_BRIGHTNESS

#include "../gcode.h"
#include "../../lcd/mvcncui.h"

/**
 * M256: Set the LCD brightness
 */
void GcodeSuite::M256() {
  if (parser.seenval('B'))
    ui.set_brightness(parser.value_int());
  else
    M256_report();
}

void GcodeSuite::M256_report(const bool forReplay/*=true*/) {
  report_heading_etc(forReplay, F(STR_LCD_BRIGHTNESS));
  SERIAL_ECHOLNPGM("  M256 B", ui.brightness);
}

#endif // HAS_LCD_BRIGHTNESS
