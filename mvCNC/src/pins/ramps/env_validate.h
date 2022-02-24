/**
 * Modern Vintage CNC Firmware
 * Copyright (c) 2022 Edward Webber [https://github.com/Domush/mvCNC-Modern-Vintage-CNC-Firmware]

 * Based on Marlin and grbl.
 */
#pragma once

#if ENABLED(ALLOW_SAM3X8E)
  #if NOT_TARGET(__SAM3X8E__, __AVR_ATmega2560__)
    #error "Oops! Select 'Arduino Due' or 'Arduino/Genuino Mega or Mega 2560' in 'Tools > Board.'"
  #endif
#elif ENABLED(REQUIRE_MEGA2560) && NOT_TARGET(__AVR_ATmega2560__)
  #error "Oops! Select 'Arduino/Genuino Mega or Mega 2560' in 'Tools > Board.'"
#elif DISABLED(REQUIRE_MEGA2560) && NOT_TARGET(__AVR_ATmega1280__, __AVR_ATmega2560__)
  #error "Oops! Select 'Arduino/Genuino Mega or Mega 2560 or 1280' in 'Tools > Board.'"
#endif

#undef ALLOW_SAM3X8E
#undef REQUIRE_MEGA2560
