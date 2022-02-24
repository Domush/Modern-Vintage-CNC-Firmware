/**
 * Modern Vintage CNC Firmware
 * Copyright (c) 2022 Edward Webber [https://github.com/Domush/mvCNC-Modern-Vintage-CNC-Firmware]

 * Based on Marlin and grbl.
 */
#pragma once

/**
 * Panda ZHU pin assignments
 */

#define BOARD_INFO_NAME "Panda_ZHU"

#include "pins_PANDA_common.h"

//
// Steppers
//
#define X_ENABLE_PIN                         128  // Shared with all steppers
#define Y_ENABLE_PIN                X_ENABLE_PIN
#define Z_ENABLE_PIN                X_ENABLE_PIN
#define E0_ENABLE_PIN               X_ENABLE_PIN

//#define X_CS_PIN                             0
//#define Y_CS_PIN                            13
//#define Z_CS_PIN                             5  // SS_PIN
//#define E0_CS_PIN                           21

#define E1_STEP_PIN                          115
#define E1_DIR_PIN                           114
#define E1_ENABLE_PIN               X_ENABLE_PIN

#define E2_STEP_PIN                          112
#define E2_DIR_PIN                           113
#define E2_ENABLE_PIN               X_ENABLE_PIN

#define E3_STEP_PIN                          110
#define E3_DIR_PIN                           111
#define E3_ENABLE_PIN               X_ENABLE_PIN

#define E4_STEP_PIN                          121
#define E4_DIR_PIN                           122
#define E4_ENABLE_PIN               X_ENABLE_PIN

#define HEATER_1_PIN                         123
