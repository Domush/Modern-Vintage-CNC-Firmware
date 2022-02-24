/**
 * Modern Vintage CNC Firmware
 *
 * Copyright (c) 2022 Edward Webber [https://github.com/Domush/mvCNC-Modern-Vintage-CNC-Firmware]

 * Based on Marlin and grbl.
 */
#pragma once

// Initialize watchdog with a 4 second interrupt time
void watchdog_init();

// Reset watchdog. MUST be called at least every 4 seconds after the
// first watchdog_init or SAMD will go into emergency procedures.
inline void HAL_watchdog_refresh() {
  SYNC(WDT->SYNCBUSY.bit.CLEAR);        // Test first if previous is 'ongoing' to save time waiting for command execution
  WDT->CLEAR.reg = WDT_CLEAR_CLEAR_KEY;
}
