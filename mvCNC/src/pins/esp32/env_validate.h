/**
 * Modern Vintage CNC Firmware
 * Copyright (c) 2022 Edward Webber [https://github.com/Domush/mvCNC-Modern-Vintage-CNC-Firmware]

 * Based on Marlin and grbl.
 */
#pragma once

#if NOT_TARGET(ARDUINO_ARCH_ESP32)
  #error "Oops! Select an ESP32 board in 'Tools > Board.'"
#endif
