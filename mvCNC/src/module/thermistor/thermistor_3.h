/**
 * Modern Vintage CNC Firmware
*/
#pragma once

// R25 = 100 kOhm, beta25 = 4120 K, 4.7 kOhm pull-up, mendel-parts
constexpr temp_entry_t temptable_3[] PROGMEM = {
  { OV(   1), 864 },
  { OV(  21), 300 },
  { OV(  25), 290 },
  { OV(  29), 280 },
  { OV(  33), 270 },
  { OV(  39), 260 },
  { OV(  46), 250 },
  { OV(  54), 240 },
  { OV(  64), 230 },
  { OV(  75), 220 },
  { OV(  90), 210 },
  { OV( 107), 200 },
  { OV( 128), 190 },
  { OV( 154), 180 },
  { OV( 184), 170 },
  { OV( 221), 160 },
  { OV( 265), 150 },
  { OV( 316), 140 },
  { OV( 375), 130 },
  { OV( 441), 120 },
  { OV( 513), 110 },
  { OV( 588), 100 },
  { OV( 734),  80 },
  { OV( 856),  60 },
  { OV( 938),  40 },
  { OV( 986),  20 },
  { OV(1008),   0 },
  { OV(1018), -20 }
};
