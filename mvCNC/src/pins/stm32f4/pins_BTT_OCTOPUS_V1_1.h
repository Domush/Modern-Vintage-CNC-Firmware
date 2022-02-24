/**
 * Modern Vintage CNC Firmware
 * Copyright (c) 2022 Edward Webber [https://github.com/Domush/mvCNC-Modern-Vintage-CNC-Firmware]

 * Based on Marlin and grbl.
 */
#pragma once

#define BOARD_INFO_NAME "BTT OCTOPUS V1.1"

//
// Temperature Sensors
//
#if TEMP_SENSOR_0 == 20
  #define TEMP_0_PIN                        PF8   // PT100 Connector
#else
  #define TEMP_0_PIN                        PF4   // TH0
#endif

#include "pins_BTT_OCTOPUS_V1_common.h"
