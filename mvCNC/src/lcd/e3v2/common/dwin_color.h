/**
 * Modern Vintage CNC Firmware
 * Copyright (c) 2022 Edward Webber [https://github.com/Domush/mvCNC-Modern-Vintage-CNC-Firmware]

 * Based on Marlin and grbl.
 */
#pragma once

// Extended and default UI Colors
#define RGB(R,G,B)  (R << 11) | (G << 5) | (B) // R,B: 0..31; G: 0..63
#define GetRColor(color) ((color >> 11) & 0x1F)
#define GetGColor(color) ((color >>  5) & 0x3F)
#define GetBColor(color) ((color >>  0) & 0x1F)

#define Color_White         0xFFFF
#define Color_Yellow        RGB(0x1F,0x3F,0x00)
#define Color_Red           RGB(0x1F,0x00,0x00)
#define Color_Error_Red     0xB000  // Error!
#define Color_Bg_Red        0xF00F  // Red background color
#define Color_Bg_Window     0x31E8  // Popup background color
#define Color_Bg_Blue       0x1125  // Dark blue background color
#define Color_Bg_Black      0x0841  // Black background color
#define Color_IconBlue      0x45FA  // Lighter blue that matches icons/accents
#define Popup_Text_Color    0xD6BA  // Popup font background color
#define Line_Color          0x3A6A  // Split line color
#define Rectangle_Color     0xEE2F  // Blue square cursor color
#define Percent_Color       0xFE29  // Percentage color
#define BarFill_Color       0x10E4  // Fill color of progress bar
#define Select_Color        0x33BB  // Selected color
