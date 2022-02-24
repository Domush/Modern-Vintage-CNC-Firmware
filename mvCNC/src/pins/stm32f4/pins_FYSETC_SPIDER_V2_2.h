/**
 * Modern Vintage CNC Firmware
 * Copyright (c) 2022 Edward Webber [https://github.com/Domush/mvCNC-Modern-Vintage-CNC-Firmware]

 * Based on Marlin and grbl.
 */
#pragma once

#define BOARD_INFO_NAME "FYSETC SPIDER V22"
#define DEFAULT_MACHINE_NAME BOARD_INFO_NAME

#define TEMP_3_PIN                          PC3
#define TEMP_4_PIN                          PB1
#define TEMP_BED_PIN                        PB0

#define FAN_PIN                             PA13
#define FAN1_PIN                            PA14

#include "pins_FYSETC_SPIDER.h"
