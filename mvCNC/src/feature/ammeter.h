/**
 * Modern Vintage CNC Firmware
 * Copyright (c) 2022 Edward Webber [https://github.com/Domush/mvCNC-Modern-Vintage-CNC-Firmware]

 * Based on Marlin and grbl.
 */
#pragma once

#include "../inc/mvCNCConfigPre.h"

#include <Wire.h>
#include <INA226.h>

class Ammeter {
private:
  static float scale;

public:
  static float current;
  static void init();
  static float read();
};

extern Ammeter ammeter;
