/**
 * Modern Vintage CNC Firmware
 * Copyright (c) 2022 Edward Webber [https://github.com/Domush/mvCNC-Modern-Vintage-CNC-Firmware]

 * Based on Marlin and grbl.
 */
#pragma once

#define BOARD_INFO_NAME "BTT OCTOPUS PRO V1.0"

//
// Temperature Sensors
//
#if TEMP_SENSOR_0 == -5
  #define TEMP_0_CS_PIN                     PF8   // Max31865 CS
  #define TEMP_0_SCK_PIN                    PA5
  #define TEMP_0_MISO_PIN                   PA6
  #define TEMP_0_MOSI_PIN                   PA7
  #define SOFTWARE_SPI                            // Max31865 and LCD SD share a set of SPIs, Set SD to softwareSPI for Max31865
  #define FORCE_SOFT_SPI
#else
  #define TEMP_0_PIN                        PF4   // TH0
#endif

#if !defined(Z_MIN_PROBE_PIN) && DISABLED(BLTOUCH)
  #define Z_MIN_PROBE_PIN                   PC5   // Probe (Proximity switch) port
#endif

#include "pins_BTT_OCTOPUS_V1_common.h"
