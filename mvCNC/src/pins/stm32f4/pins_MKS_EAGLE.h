/**
 * Modern Vintage CNC Firmware
 * Copyright (c) 2022 Edward Webber [https://github.com/Domush/mvCNC-Modern-Vintage-CNC-Firmware]

 * Based on Marlin and grbl.
 */
#pragma once

#define ALLOW_STM32DUINO
#include "env_validate.h"

#if HOTENDS > 2 || E_STEPPERS > 2
  #error "MKS Eagle supports up to 2 hotends / E-steppers."
#elif HAS_FSMC_TFT
  #error "MKS Eagle doesn't support FSMC-based TFT displays."
#endif

#define BOARD_INFO_NAME "MKS Eagle"

#include "pins_MKS_ROBIN_NANO_V3_common.h"
