/**
 * Modern Vintage CNC Firmware
*/
#pragma once

#include "../../../inc/mvCNCConfig.h"

#if ENABLED(TOUCH_BUTTONS_HW_SPI)
  #include <SPI.h>
#endif

#ifndef TOUCH_MISO_PIN
  #define TOUCH_MISO_PIN SD_MISO_PIN
#endif
#ifndef TOUCH_MOSI_PIN
  #define TOUCH_MOSI_PIN SD_MOSI_PIN
#endif
#ifndef TOUCH_SCK_PIN
  #define TOUCH_SCK_PIN  SD_SCK_PIN
#endif
#ifndef TOUCH_CS_PIN
  #define TOUCH_CS_PIN   SD_SS_PIN
#endif
#ifndef TOUCH_INT_PIN
  #define TOUCH_INT_PIN  -1
#endif

#define XPT2046_DFR_MODE        0x00
#define XPT2046_SER_MODE        0x04
#define XPT2046_CONTROL         0x80

enum XPTCoordinate : uint8_t {
  XPT2046_X  = 0x10 | XPT2046_CONTROL | XPT2046_DFR_MODE,
  XPT2046_Y  = 0x50 | XPT2046_CONTROL | XPT2046_DFR_MODE,
  XPT2046_Z1 = 0x30 | XPT2046_CONTROL | XPT2046_DFR_MODE,
  XPT2046_Z2 = 0x40 | XPT2046_CONTROL | XPT2046_DFR_MODE,
};

#ifndef XPT2046_Z1_THRESHOLD
  #define XPT2046_Z1_THRESHOLD 10
#endif

class XPT2046 {
private:
  static bool isBusy() { return false; }

  static uint16_t getRawData(const XPTCoordinate coordinate);
  static bool isTouched();

  static void DataTransferBegin() { WRITE(TOUCH_CS_PIN, LOW); };
  static void DataTransferEnd() { WRITE(TOUCH_CS_PIN, HIGH); };
  #if ENABLED(TOUCH_BUTTONS_HW_SPI)
    static uint16_t HardwareIO(uint16_t data);
  #endif
  static uint16_t SoftwareIO(uint16_t data);
  static uint16_t IO(uint16_t data = 0);

public:
  #if ENABLED(TOUCH_BUTTONS_HW_SPI)
    static SPIClass SPIx;
  #endif

  static void Init();
  static bool getRawPoint(int16_t *x, int16_t *y);
};
