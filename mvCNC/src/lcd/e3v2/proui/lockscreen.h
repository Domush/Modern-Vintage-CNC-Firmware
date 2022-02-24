/**
 * Modern Vintage CNC Firmware
 * Copyright (c) 2022 Edward Webber [https://github.com/Domush/mvCNC-Modern-Vintage-CNC-Firmware]

 * Based on Marlin and grbl.
 */
#pragma once

/**
 * Lock screen implementation for DWIN UI Enhanced implementation
 * Author: Miguel A. Risco-Castillo (MRISCOC)
 * Version: 2.1
 * Date: 2021/11/09
 *
 * Based on the original code provided by Creality under GPL
 */

#include "../common/encoder.h"
#include <stdint.h>

class LockScreenClass {
private:
  static bool unlocked;
  static uint8_t lock_pos;
public:
  static uint8_t rprocess;
  static void init();
  static void onEncoder(EncoderState encoder_diffState);
  static void draw();
  static bool isUnlocked() { return unlocked; }
};

extern LockScreenClass lockScreen;
