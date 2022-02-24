/**
 * Modern Vintage CNC Firmware
 * Copyright (c) 2022 Edward Webber [https://github.com/Domush/mvCNC-Modern-Vintage-CNC-Firmware]

 * Based on Marlin and grbl.
 */
#include "HAL_MinSerial.h"

#if ENABLED(POSTMORTEM_DEBUGGING)

void HAL_min_serial_init_default() {}
void HAL_min_serial_out_default(char ch) { SERIAL_CHAR(ch); }
void (*HAL_min_serial_init)() = &HAL_min_serial_init_default;
void (*HAL_min_serial_out)(char) = &HAL_min_serial_out_default;

bool MinSerial::force_using_default_output = false;

#endif
