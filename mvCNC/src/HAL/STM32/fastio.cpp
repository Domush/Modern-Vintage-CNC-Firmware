/**
 * Modern Vintage CNC Firmware
 * Copyright (c) 2022 Edward Webber [https://github.com/Domush/mvCNC-Modern-Vintage-CNC-Firmware]

 * Based on Marlin and grbl.
 */
#include "../platforms.h"

#ifdef HAL_STM32

#include "../../inc/mvCNCConfig.h"

GPIO_TypeDef* FastIOPortMap[LastPort + 1] = { 0 };

void FastIO_init() {
  LOOP_L_N(i, NUM_DIGITAL_PINS)
    FastIOPortMap[STM_PORT(digitalPin[i])] = get_GPIO_Port(STM_PORT(digitalPin[i]));
}

#endif // HAL_STM32
