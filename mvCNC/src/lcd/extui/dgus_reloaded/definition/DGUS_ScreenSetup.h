/**
 * Modern Vintage CNC Firmware
 * Copyright (c) 2022 Edward Webber [https://github.com/Domush/mvCNC-Modern-Vintage-CNC-Firmware]

 * Based on Marlin and grbl.
 */
#pragma once

#include "../config/DGUS_Screen.h"

struct DGUS_ScreenSetup {
  DGUS_Screen   screen;
  bool          (*setup_fn)(void);
};

extern const struct DGUS_ScreenSetup screen_setup_list[];
