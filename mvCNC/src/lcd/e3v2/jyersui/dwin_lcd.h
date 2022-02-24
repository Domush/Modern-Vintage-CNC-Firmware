/**
 * Modern Vintage CNC Firmware
 * Copyright (c) 2022 Edward Webber [https://github.com/Domush/mvCNC-Modern-Vintage-CNC-Firmware]

 * Based on Marlin and grbl.
 */
#pragma once

/********************************************************************************
 * @file     lcd/e3v2/jyersui/dwin_lcd.h
 * @brief    DWIN screen control functions
 ********************************************************************************/

#include "../common/dwin_api.h"

// Draw the degree (°) symbol
// Color: color
//  x/y: Upper-left coordinate of the first pixel
void DWIN_Draw_DegreeSymbol(uint16_t Color, uint16_t x, uint16_t y);
