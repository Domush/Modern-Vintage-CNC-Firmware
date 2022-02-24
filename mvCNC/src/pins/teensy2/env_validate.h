/**
 * Modern Vintage CNC Firmware
 * Copyright (c) 2022 Edward Webber [https://github.com/Domush/mvCNC-Modern-Vintage-CNC-Firmware]

 * Based on Marlin and grbl.
 */
#pragma once

#if NOT_TARGET(__AVR_AT90USB1286__) && (DISABLED(ALLOW_AT90USB1286P) || NOT_TARGET(__AVR_AT90USB1286P__))
  #error "Oops! Select 'Teensy++ 2.0' or 'Printrboard' in 'Tools > Board.'"
#endif

#undef ALLOW_AT90USB1286P
