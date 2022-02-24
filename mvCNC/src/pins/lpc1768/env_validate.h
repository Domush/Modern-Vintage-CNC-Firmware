/**
 * Modern Vintage CNC Firmware
 * Copyright (c) 2022 Edward Webber [https://github.com/Domush/mvCNC-Modern-Vintage-CNC-Firmware]

 * Based on Marlin and grbl.
 */
#pragma once

#if ENABLED(REQUIRE_LPC1769) && NOT_TARGET(MCU_LPC1769)
  #error "Oops! Make sure you have the LPC1769 environment selected in your IDE."
#elif DISABLED(REQUIRE_LPC1769) && NOT_TARGET(MCU_LPC1768)
  #error "Oops! Make sure you have the LPC1768 environment selected in your IDE."
#endif

#undef REQUIRE_LPC1769
