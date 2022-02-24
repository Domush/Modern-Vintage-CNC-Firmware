/**
 * Modern Vintage CNC Firmware
 * Copyright (c) 2022 Edward Webber [https://github.com/Domush/mvCNC-Modern-Vintage-CNC-Firmware]

 * Based on Marlin and grbl.
 */
#pragma once

#define WDT_TIMEOUT   4000000 // 4 second timeout

void watchdog_init();
void HAL_watchdog_refresh();
