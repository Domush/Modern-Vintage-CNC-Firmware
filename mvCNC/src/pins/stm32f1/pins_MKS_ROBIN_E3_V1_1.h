/**
 * Modern Vintage CNC Firmware
*/
#pragma once

/**
 * MKS Robin E3 v1.1 (STM32F103RCT6) board pin assignments
 */

#if TOOL_CHANGE_SUPPORT || E_STEPPERS > 1
  #error "MKS Robin E3 v1.1 only supports one hotend / E-stepper. Comment out this line to continue."
#endif

#ifndef BOARD_INFO_NAME
  #define BOARD_INFO_NAME "MKS Robin E3 V1.1"
#endif

#include "pins_MKS_ROBIN_E3_V1_1_common.h" // ... MKS_ROBIN_E3_common
