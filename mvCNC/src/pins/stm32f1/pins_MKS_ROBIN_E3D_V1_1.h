/**
 * Modern Vintage CNC Firmware
*/
#pragma once

/**
 * MKS Robin E3D v1.1 (STM32F103RCT6) board pin assignments
 */

#if TOOL_CHANGE_SUPPORT || E_STEPPERS > 1
  #error "MKS Robin E3D v1.1 only supports one hotend / E-stepper. Comment out this line to continue."
#endif

#ifndef BOARD_INFO_NAME
  #define BOARD_INFO_NAME "MKS Robin E3D V1.1"
#endif

//
// Steppers
//
#ifndef X_CS_PIN
  #define X_CS_PIN                          PC7
#endif
#ifndef Y_CS_PIN
  #define Y_CS_PIN                          PD2
#endif
#ifndef Z_CS_PIN
  #define Z_CS_PIN                          PC12
#endif
#ifndef E0_CS_PIN
  #define E0_CS_PIN                         PC11
#endif

//
// Software SPI pins for TMC2130 stepper drivers
//
#if ENABLED(TMC_USE_SW_SPI)
  #ifndef TMC_SW_MOSI
    #define TMC_SW_MOSI                     PB15
  #endif
  #ifndef TMC_SW_MISO
    #define TMC_SW_MISO                     PB14
  #endif
  #ifndef TMC_SW_SCK
    #define TMC_SW_SCK                      PB13
  #endif
#endif

#include "pins_MKS_ROBIN_E3_V1_1_common.h" // ... MKS_ROBIN_E3_common
