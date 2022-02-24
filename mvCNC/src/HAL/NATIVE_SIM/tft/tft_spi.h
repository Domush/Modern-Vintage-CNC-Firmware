/**
 * Modern Vintage CNC Firmware
 * Copyright (c) 2022 Edward Webber [https://github.com/Domush/mvCNC-Modern-Vintage-CNC-Firmware]

 * Based on Marlin and grbl.
 */
#pragma once

#include "../../../inc/mvCNCConfig.h"

#ifndef LCD_READ_ID
  #define LCD_READ_ID  0x04   // Read display identification information (0xD3 on ILI9341)
#endif
#ifndef LCD_READ_ID4
  #define LCD_READ_ID4 0xD3   // Read display identification information (0xD3 on ILI9341)
#endif

#define DATASIZE_8BIT    8
#define DATASIZE_16BIT   16
#define TFT_IO_DRIVER TFT_SPI

#define DMA_MINC_ENABLE 1
#define DMA_MINC_DISABLE 0

class TFT_SPI {
private:
  static uint32_t ReadID(uint16_t Reg);
  static void Transmit(uint16_t Data);
  static void TransmitDMA(uint32_t MemoryIncrease, uint16_t *Data, uint16_t Count);

public:
  // static SPIClass SPIx;

  static void Init();
  static uint32_t GetID();
  static bool isBusy();
  static void Abort();

  static void DataTransferBegin(uint16_t DataWidth = DATASIZE_16BIT);
  static void DataTransferEnd();
  static void DataTransferAbort();

  static void WriteData(uint16_t Data);
  static void WriteReg(uint16_t Reg);

  static void WriteSequence(uint16_t *Data, uint16_t Count);
  // static void WriteMultiple(uint16_t Color, uint16_t Count);
  static void WriteMultiple(uint16_t Color, uint32_t Count);
};
