/**
 * Modern Vintage CNC Firmware
 * Copyright (c) 2022 Edward Webber [https://github.com/Domush/mvCNC-Modern-Vintage-CNC-Firmware]

 * Based on Marlin and grbl.
 */
#pragma once

#if BOTH(ALLOW_MEGA1280, ALLOW_MEGA2560) && NOT_TARGET(__SAM3X8E__, __AVR_ATmega1280__, __AVR_ATmega2560__)
  #error "Oops! Select 'Arduino Due or Mega' in 'Tools > Board.'"
#elif ENABLED(ALLOW_MEGA2560) && NOT_TARGET(__SAM3X8E__, __AVR_ATmega2560__)
  #error "Oops! Select 'Arduino Due or Mega' in 'Tools > Board.'"
#elif ENABLED(ALLOW_MEGA1280) && NOT_TARGET(__SAM3X8E__, __AVR_ATmega1280__)
  #error "Oops! Select 'Arduino Due' in 'Tools > Board.'"
#endif

#undef ALLOW_MEGA1280
#undef ALLOW_MEGA2560
