/**
 * Modern Vintage CNC Firmware
 *
 * Copyright (c) 2022 Edward Webber [https://github.com/Domush/mvCNC-Modern-Vintage-CNC-Firmware]

 * Based on Marlin and grbl.
 */
#pragma once

#include <USBComposite.h>

#include "../../inc/mvCNCConfigPre.h"
#include "../../core/serial_hook.h"

extern USBMassStorage mvCNCMSC;
extern Serial1Class<USBCompositeSerial> mvCNCCompositeSerial;

void MSC_SD_init();
