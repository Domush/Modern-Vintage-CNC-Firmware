/**
 * Modern Vintage CNC Firmware
*/

#include "../../../inc/mvCNCConfigPre.h"

#if HAS_GRAPHICAL_TFT

extern const uint8_t directory_32x32x4[512] = {
  0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88,
  0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88,
  0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88,
  0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88,
  0x88, 0x88, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88,
  0x88, 0x87, 0x9F, 0xFF, 0xFF, 0xFF, 0xFF, 0xF7, 0x78, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88,
  0x88, 0x87, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFA, 0x78, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88,
  0x88, 0x87, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x68, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88,
  0x88, 0x87, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x87, 0x77, 0x77, 0x77, 0x77, 0x77, 0x88, 0x88,
  0x88, 0x87, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFA, 0x78, 0x88,
  0x88, 0x87, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x78, 0x88,
  0x88, 0x87, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x68, 0x88,
  0x88, 0x87, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x68, 0x88,
  0x88, 0x88, 0x76, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x68, 0x88,
  0x88, 0x87, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x68, 0x88,
  0x88, 0x87, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x68, 0x88,
  0x88, 0x87, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x68, 0x88,
  0x88, 0x87, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x68, 0x88,
  0x88, 0x87, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x68, 0x88,
  0x88, 0x87, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x68, 0x88,
  0x88, 0x87, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x68, 0x88,
  0x88, 0x87, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x68, 0x88,
  0x88, 0x87, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x68, 0x88,
  0x88, 0x87, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x68, 0x88,
  0x88, 0x87, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x68, 0x88,
  0x88, 0x87, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x68, 0x88,
  0x88, 0x87, 0xBF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFC, 0x68, 0x88,
  0x88, 0x88, 0x77, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x86, 0x68, 0x88,
  0x88, 0x88, 0x87, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x67, 0x88, 0x88,
  0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88,
  0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88,
  0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88
};

#endif // HAS_GRAPHICAL_TFT