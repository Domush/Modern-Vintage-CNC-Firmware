/**
 * Modern Vintage CNC Firmware
 * Copyright (c) 2022 Edward Webber [https://github.com/Domush/mvCNC-Modern-Vintage-CNC-Firmware]

 * Based on Marlin and grbl.
 */
#pragma once

#if NOT_TARGET(__AVR_ATmega2560__)
  #if DISABLED(ALLOW_MEGA1280)
    #error "Oops! Select 'Arduino/Genuino Mega or Mega 2560' in 'Tools > Board.'"
  #elif NOT_TARGET(__AVR_ATmega1280__)
    #error "Oops! Select 'Arduino/Genuino Mega or Mega 2560 or 1280' in 'Tools > Board.'"
  #endif
#endif

#undef ALLOW_MEGA1280
