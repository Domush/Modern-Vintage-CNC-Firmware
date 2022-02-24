/**
 * Modern Vintage CNC Firmware
 *
 * Copyright (c) 2022 Edward Webber [https://github.com/Domush/mvCNC-Modern-Vintage-CNC-Firmware]

 * Based on Marlin and grbl.
 */
#pragma once

#include "../../core/serial_hook.h"

typedef Serial1Class<Uart> UartT;

extern UartT Serial2;
extern UartT Serial3;
extern UartT Serial4;
