/**
 * Modern Vintage CNC Firmware
 *
 * Copyright (c) 2022 Edward Webber [https://github.com/Domush/mvCNC-Modern-Vintage-CNC-Firmware]

 * Based on Marlin and grbl.
 */
#ifdef __SAMD51__

#include "../../inc/mvCNCConfig.h"

#if ENABLED(USE_WATCHDOG)

#include "watchdog.h"

#define WDT_TIMEOUT_REG TERN(WATCHDOG_DURATION_8S, WDT_CONFIG_PER_CYC8192, WDT_CONFIG_PER_CYC4096) // 4 or 8 second timeout

void watchdog_init() {
  // The low-power oscillator used by the WDT runs at 32,768 Hz with
  // a 1:32 prescale, thus 1024 Hz, though probably not super precise.

  // Setup WDT clocks
  MCLK->APBAMASK.bit.OSC32KCTRL_ = true;
  MCLK->APBAMASK.bit.WDT_ = true;
  OSC32KCTRL->OSCULP32K.bit.EN1K = true;      // Enable out 1K (this is what WDT uses)

  WDT->CTRLA.bit.ENABLE = false;              // Disable watchdog for config
  SYNC(WDT->SYNCBUSY.bit.ENABLE);

  WDT->INTENCLR.reg = WDT_INTENCLR_EW;        // Disable early warning interrupt
  WDT->CONFIG.reg = WDT_TIMEOUT_REG;          // Set a 4s or 8s period for chip reset

  HAL_watchdog_refresh();

  WDT->CTRLA.reg = WDT_CTRLA_ENABLE;          // Start watchdog now in normal mode
  SYNC(WDT->SYNCBUSY.bit.ENABLE);
}

#endif // USE_WATCHDOG

#endif // __SAMD51__
