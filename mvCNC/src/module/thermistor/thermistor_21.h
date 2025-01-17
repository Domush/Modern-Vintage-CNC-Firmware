/**
 * Modern Vintage CNC Firmware
*/
#pragma once

#define REVERSE_TEMP_SENSOR_RANGE_21 1

#undef OV_SCALE
#define OV_SCALE(N) (float((N) * 5) / 3.3f)

// Pt100 with INA826 amplifier board with 5v supply based on Thermistor 20, with 3v3 ADC reference on the mainboard.
// If the ADC reference and INA826 board supply voltage are identical, Thermistor 20 instead.
constexpr temp_entry_t temptable_21[] PROGMEM = {
  { OV(  0),    0 },
  { OV(227),    1 },
  { OV(236),   10 },
  { OV(245),   20 },
  { OV(253),   30 },
  { OV(262),   40 },
  { OV(270),   50 },
  { OV(279),   60 },
  { OV(287),   70 },
  { OV(295),   80 },
  { OV(304),   90 },
  { OV(312),  100 },
  { OV(320),  110 },
  { OV(329),  120 },
  { OV(337),  130 },
  { OV(345),  140 },
  { OV(353),  150 },
  { OV(361),  160 },
  { OV(369),  170 },
  { OV(377),  180 },
  { OV(385),  190 },
  { OV(393),  200 },
  { OV(401),  210 },
  { OV(409),  220 },
  { OV(417),  230 },
  { OV(424),  240 },
  { OV(432),  250 },
  { OV(440),  260 },
  { OV(447),  270 },
  { OV(455),  280 },
  { OV(463),  290 },
  { OV(470),  300 },
  { OV(478),  310 },
  { OV(485),  320 },
  { OV(493),  330 },
  { OV(500),  340 },
  { OV(507),  350 },
  { OV(515),  360 },
  { OV(522),  370 },
  { OV(529),  380 },
  { OV(537),  390 },
  { OV(544),  400 },
  { OV(614),  500 }
};

#undef OV_SCALE
#define OV_SCALE(N) (N)
