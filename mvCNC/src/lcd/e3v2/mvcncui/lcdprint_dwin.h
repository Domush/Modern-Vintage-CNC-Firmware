/**
 * Modern Vintage CNC Firmware
 * Copyright (c) 2022 Edward Webber [https://github.com/Domush/mvCNC-Modern-Vintage-CNC-Firmware]

 * Based on Marlin and grbl.
 */
#pragma once

#include "../../lcdprint.h"

typedef struct { int16_t x, y; } cursor_t;
extern cursor_t cursor;

int lcd_put_dwin_string();
void lcd_moveto_xy(const lcd_uint_t, const lcd_uint_t);
