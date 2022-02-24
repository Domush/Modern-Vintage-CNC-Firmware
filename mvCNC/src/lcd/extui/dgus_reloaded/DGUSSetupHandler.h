/**
 * Modern Vintage CNC Firmware
 * Copyright (c) 2022 Edward Webber [https://github.com/Domush/mvCNC-Modern-Vintage-CNC-Firmware]

 * Based on Marlin and grbl.
 */
#pragma once

namespace DGUSSetupHandler {

  #if ENABLED(SDSUPPORT)
    bool CNC();
  #endif
  bool CNCStatus();
  bool CNCAdjust();
  bool LevelingMenu();
  bool LevelingOffset();
  bool LevelingManual();
  bool LevelingAutomatic();
  bool LevelingProbing();
  bool Filament();
  bool Move();
  bool Gcode();
  bool PID();
  bool Infos();

}
