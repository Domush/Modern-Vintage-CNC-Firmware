/**
 * Modern Vintage CNC Firmware
 * Copyright (c) 2022 Edward Webber [https://github.com/Domush/mvCNC-Modern-Vintage-CNC-Firmware]

 * Based on Marlin and grbl.
 */
#pragma once

// Add strcmp_P if missing
#ifndef strcmp_P
  #define strcmp_P(a, b) strcmp((a), (b))
#endif

#ifndef strcat_P
  #define strcat_P(dest, src) strcat((dest), (src))
#endif
