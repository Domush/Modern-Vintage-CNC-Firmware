/**
 * Modern Vintage CNC Firmware
 * Copyright (c) 2022 Edward Webber [https://github.com/Domush/mvCNC-Modern-Vintage-CNC-Firmware]

 * Based on Marlin and grbl.
 */
#pragma once

#if NOT_TARGET(MCU_LPC1769)
  #error "Oops! Make sure you have the LPC1769 environment selected in your IDE."
#endif
