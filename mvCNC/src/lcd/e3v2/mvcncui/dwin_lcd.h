/**
 * Modern Vintage CNC Firmware
 * Copyright (c) 2022 Edward Webber [https://github.com/Domush/mvCNC-Modern-Vintage-CNC-Firmware]

 * Based on Marlin and grbl.
 */
#pragma once

/********************************************************************************
 * @file     lcd/e3v2/mvcncui/dwin_lcd.h
 * @brief    DWIN screen control functions
 ********************************************************************************/

#include "../../../inc/mvCNCConfigPre.h"

#include "../common/dwin_api.h"

// Picture ID
#define DWIN_Boot_Horiz      0
#define DWIN_Boot_Vert       1
#define DWIN_mvCNCUI_Assets 2

// ICON ID
#define BOOT_ICON           3 // Icon set file 3.ICO
#define ICON                4 // Icon set file 4.ICO

// mvCNCUI Boot Icons from Set 3
#define ICON_mvCNCBoot            0
#define ICON_OpenSource            1
#define ICON_GitHubURL             2
#define ICON_mvCNCURL             3
#define ICON_Copyright             4

// mvCNCUI Icons from Set 4
#define ICON_LOGO_mvCNC           0
#define ICON_HotendOff             1
#define ICON_HotendOn              2
#define ICON_BedOff                3
#define ICON_BedOn                 4
#define ICON_Fan0                  5
#define ICON_Fan1                  6
#define ICON_Fan2                  7
#define ICON_Fan3                  8
#define ICON_Halted                9
#define ICON_Question             10
#define ICON_Alert                11
#define ICON_RotateCW             12
#define ICON_RotateCCW            13
#define ICON_UpArrow              14
#define ICON_DownArrow            15
#define ICON_BedLine              16
#define ICON_BedLevelOff          17
#define ICON_BedLevelOn           18

#include "../common/dwin_font.h"

#define DWIN_FONT_MENU  font10x20
#define DWIN_FONT_STAT  font14x28
#define DWIN_FONT_ALERT font14x28

#include "../common/dwin_color.h"

#define Color_Bg_Heading  0x3344  // Static Heading

// Character matrix width x height
//#define LCD_WIDTH ((DWIN_WIDTH) / 8)
//#define LCD_HEIGHT ((DWIN_HEIGHT) / 12)