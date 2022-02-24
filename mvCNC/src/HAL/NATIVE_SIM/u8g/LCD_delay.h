/**
 * Modern Vintage CNC Firmware
 * Copyright (c) 2022 Edward Webber [https://github.com/Domush/mvCNC-Modern-Vintage-CNC-Firmware]

 * Based on Marlin and grbl.
 */
#pragma once

/**
 * LCD delay routines - used by all the drivers.
 *
 * These are based on the LPC1768 routines.
 *
 * Couldn't just call exact copies because the overhead
 * results in a one microsecond delay taking about 4µS.
 */

#ifdef __cplusplus
  extern "C" {
#endif

void U8g_delay(int msec);
void u8g_MicroDelay();
void u8g_10MicroDelay();

#ifdef __cplusplus
  }
#endif
