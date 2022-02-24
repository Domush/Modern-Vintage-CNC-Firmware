/**
 * Modern Vintage CNC Firmware
 * Copyright (c) 2022 Edward Webber [https://github.com/Domush/mvCNC-Modern-Vintage-CNC-Firmware]

 * Based on Marlin and grbl.
 */
#pragma once

/**
 * DWIN End Stops diagnostic page
 * Author: Miguel A. Risco-Castillo (MRISCOC)
 * Version: 1.0
 * Date: 2021/11/06
 *
 * Based on the original code provided by Creality under GPL
 */

class ESDiagClass {
public:
  void Draw();
  void Update();
};

extern ESDiagClass ESDiag;
