/**
 * Modern Vintage CNC Firmware
 * Copyright (c) 2022 Edward Webber [https://github.com/Domush/mvCNC-Modern-Vintage-CNC-Firmware]

 * Based on Marlin and grbl.
 */
#pragma once

/**
 * DWIN UI Enhanced implementation
 * Author: Miguel A. Risco-Castillo (MRISCOC)
 * Version: 3.8.1
 * Date: 2021/11/09
 *
 * Based on the original code provided by Creality under GPL
 */

#include "../common/dwin_api.h"

// Display QR code
//  The size of the QR code is (46*QR_Pixel)*(46*QR_Pixel) dot matrix
//  QR_Pixel: The pixel size occupied by each point of the QR code: 0x01-0x0F (1-16)
//  (Nx, Ny): The coordinates of the upper left corner displayed by the QR code
//  str: multi-bit data
void DWIN_Draw_QR(uint8_t QR_Pixel, uint16_t x, uint16_t y, char *string);

inline void DWIN_Draw_QR(uint8_t QR_Pixel, uint16_t x, uint16_t y, FSTR_P title) {
  DWIN_Draw_QR(QR_Pixel, x, y, (char *)title);
}

// Copy area from virtual display area to current screen
//  cacheID: virtual area number
//  xStart/yStart: Upper-left of virtual area
//  xEnd/yEnd: Lower-right of virtual area
//  x/y: Screen paste point
void DWIN_Frame_AreaCopy(uint8_t cacheID, uint16_t xStart, uint16_t yStart, uint16_t xEnd, uint16_t yEnd, uint16_t x, uint16_t y);

// Copy area from current virtual display area to current screen
//  xStart/yStart: Upper-left of virtual area
//  xEnd/yEnd: Lower-right of virtual area
//  x/y: Screen paste point
void DWIN_Frame_AreaCopy(uint16_t xStart, uint16_t yStart, uint16_t xEnd, uint16_t yEnd, uint16_t x, uint16_t y);

// Copy area from virtual display area to current screen
//  IBD: background display: 0=Background filtering is not displayed, 1=Background display \\When setting the background filtering not to display, the background must be pure black
//  BIR: Background image restoration: 0=Background image is not restored, 1=Automatically use virtual display area image for background restoration
//  BFI: Background filtering strength: 0=normal, 1=enhanced, (only valid when the icon background display=0)
//  cacheID: virtual area number
//  xStart/yStart: Upper-left of virtual area
//  xEnd/yEnd: Lower-right of virtual area
//  x/y: Screen paste point
void DWIN_Frame_AreaCopy(bool IBD, bool BIR, bool BFI, uint8_t cacheID, uint16_t xStart, uint16_t yStart, uint16_t xEnd, uint16_t yEnd, uint16_t x, uint16_t y);

// Write buffer data to the SRAM or Flash
//  mem: 0x5A=32KB SRAM, 0xA5=16KB Flash
//  addr: start address
//  length: Bytes to write
//  data: address of the buffer with data
void DWIN_WriteToMem(uint8_t mem, uint16_t addr, uint16_t length, uint8_t *data);

// Write the contents of the 32KB SRAM data memory into the designated image memory space.
//  picID: Picture memory space location, 0x00-0x0F, each space is 32Kbytes
void DWIN_SRAMToPic(uint8_t picID);