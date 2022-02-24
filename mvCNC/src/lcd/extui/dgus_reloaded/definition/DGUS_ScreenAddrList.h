/**
 * Modern Vintage CNC Firmware
 * Copyright (c) 2022 Edward Webber [https://github.com/Domush/mvCNC-Modern-Vintage-CNC-Firmware]

 * Based on Marlin and grbl.
 */
#pragma once

#include "../config/DGUS_Screen.h"
#include "../config/DGUS_Addr.h"

struct DGUS_ScreenAddrList {
  DGUS_Screen       screen;
  const DGUS_Addr   *addr_list;
};

extern const struct DGUS_ScreenAddrList screen_addr_list_map[];
