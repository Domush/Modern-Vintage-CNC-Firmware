/**
 * Modern Vintage CNC Firmware
 * Copyright (c) 2022 Edward Webber [https://github.com/Domush/mvCNC-Modern-Vintage-CNC-Firmware]

 * Based on Marlin and grbl.
 */
#pragma once

#if NOT_TARGET(STM32F4) && (DISABLED(ALLOW_STM32DUINO) || NOT_TARGET(STM32F4xx))
  #error "Oops! Select an STM32F4 board in 'Tools > Board.'"
#endif

#undef ALLOW_STM32DUINO
