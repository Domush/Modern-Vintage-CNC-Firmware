/**
 * Modern Vintage CNC Firmware
 * Copyright (c) 2022 Edward Webber [https://github.com/Domush/mvCNC-Modern-Vintage-CNC-Firmware]

 * Based on Marlin and grbl.
 */
#pragma once

#if NOT_TARGET(__STM32F1__, STM32F1)
  #error "Oops! Select an STM32F1 board in 'Tools > Board.'"
#endif
