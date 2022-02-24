/**
 * Modern Vintage CNC Firmware
 * Copyright (c) 2022 Edward Webber [https://github.com/Domush/mvCNC-Modern-Vintage-CNC-Firmware]

 * Based on Marlin and grbl.
 */
#pragma once

#if NOT_TARGET(__AVR_ATmega2560__)
  #error "Oops! Select 'Arduino Mega 2560 or Rambo' in 'Tools > Board.'"
#endif
