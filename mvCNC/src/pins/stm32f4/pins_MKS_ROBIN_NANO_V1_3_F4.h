/**
 * Modern Vintage CNC Firmware
 * Copyright (c) 2022 Edward Webber [https://github.com/Domush/mvCNC-Modern-Vintage-CNC-Firmware]

 * Based on Marlin and grbl.
 */
#pragma once

/**
 * MKS Robin nano V1.3 (STM32F407VET6) board pin assignments
 * https://github.com/makerbase-mks/MKS-Robin-Nano-V1.X/tree/master/hardware
 */

#define ALLOW_STM32DUINO
#include "env_validate.h"

#define BOARD_INFO_NAME "MKS Robin Nano V1.3"

//
// EEPROM
// Use one of these or SDCard-based Emulation will be used
//
#if NO_EEPROM_SELECTED
  //#define SRAM_EEPROM_EMULATION                 // Use BackSRAM-based EEPROM emulation
  //#define FLASH_EEPROM_EMULATION                // Use Flash-based EEPROM emulation
#endif

#define LED_PIN                             PB1

#include "../stm32f1/pins_MKS_ROBIN_NANO_common.h"
