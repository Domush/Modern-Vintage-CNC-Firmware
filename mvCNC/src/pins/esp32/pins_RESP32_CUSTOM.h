/**
 * Modern Vintage CNC Firmware
 * Copyright (c) 2022 Edward Webber [https://github.com/Domush/mvCNC-Modern-Vintage-CNC-Firmware]

 * Based on Marlin and grbl.
 */
#pragma once

/**
 * Rutilea ESP32 (Tensilica Xtensa LX6) pin assignments
 */

#include "env_validate.h"

#define BOARD_INFO_NAME "Rutilea ESP32"

#include "pins_ESPA_common.h"

//
// I2S (steppers & other output-only pins)
//
#define I2S_STEPPER_STREAM
