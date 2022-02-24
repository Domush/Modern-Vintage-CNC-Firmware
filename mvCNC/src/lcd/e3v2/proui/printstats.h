/**
 * Modern Vintage CNC Firmware
 * Copyright (c) 2022 Edward Webber [https://github.com/Domush/mvCNC-Modern-Vintage-CNC-Firmware]

 * Based on Marlin and grbl.
 */
#pragma once

/**
 * DWIN CNC Stats page
 * Author: Miguel A. Risco-Castillo (MRISCOC)
 * Version: 1.1
 * Date: 2022/01/09
 *
 * Based on the original code provided by Creality under GPL
 */

class CNCStatsClass {
public:
  void Draw();
  static void Reset();
};

extern CNCStatsClass CNCStats;
