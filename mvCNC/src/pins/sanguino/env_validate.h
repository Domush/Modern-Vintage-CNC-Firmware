  /**
 * Modern Vintage CNC Firmware
 * Copyright (c) 2022 Edward Webber [https://github.com/Domush/mvCNC-Modern-Vintage-CNC-Firmware]

 * Based on Marlin and grbl.
 */
#pragma once

#if ENABLED(ALLOW_MEGA644)
  #if NOT_TARGET(__AVR_ATmega644__, __AVR_ATmega644P__, __AVR_ATmega1284P__)
    #error "Oops! Select 'Sanguino' in 'Tools > Boards' and 'ATmega644', 'ATmega644P', or 'ATmega1284P' in 'Tools > Processor.'"
  #endif
#elif ENABLED(ALLOW_MEGA644P)
  #if NOT_TARGET(__AVR_ATmega644P__, __AVR_ATmega1284P__)
    #error "Oops! Select 'Sanguino' in 'Tools > Boards' and 'ATmega644P' or 'ATmega1284P' in 'Tools > Processor.'"
  #endif
#elif ENABLED(REQUIRE_MEGA644P)
  #if NOT_TARGET(__AVR_ATmega644P__)
    #error "Oops! Select 'Sanguino' in 'Tools > Board' and 'ATmega644P' in 'Tools > Processor.'"
  #endif
#elif NOT_TARGET(__AVR_ATmega1284P__)
  #error "Oops! Select 'Sanguino' in 'Tools > Board' and 'ATmega1284P' in 'Tools > Processor.' (For PlatformIO, use 'melzi' or 'melzi_optiboot.')"
#endif

#undef ALLOW_MEGA644
#undef ALLOW_MEGA644P
#undef REQUIRE_MEGA644P
