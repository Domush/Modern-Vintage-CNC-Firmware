/**
 * Modern Vintage CNC Firmware
*/
#pragma once

//
// EEPROM
//
// Onboard I2C EEPROM
#if NO_EEPROM_SELECTED
  #define I2C_EEPROM
  #define MVCNC_EEPROM_SIZE 0x1000  // 4KB
  #undef NO_EEPROM_SELECTED
#endif

#define Z_STEP_PIN                          PC14
#define Z_DIR_PIN                           PC15

#define BTN_ENC_EN                            -1

#include "pins_MKS_ROBIN_E3_common.h"
