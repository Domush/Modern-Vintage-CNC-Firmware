/**
 * Modern Vintage CNC Firmware
 * Copyright (c) 2022 Edward Webber [https://github.com/Domush/mvCNC-Modern-Vintage-CNC-Firmware]

 * Based on Marlin and grbl.
 */
#pragma once

// R25 = 100 KOhm, beta25 = 4550 K, 4.7 kOhm pull-up, TDK NTCG104LH104KT1 https://product.tdk.com/en/search/sensor/ntc/chip-ntc-thermistor/info?part_no=NTCG104LH104KT1
constexpr temp_entry_t temptable_2000[] PROGMEM = {
{ OV(313),   125 },
{ OV(347),   120 },
{ OV(383),   115 },
{ OV(422),   110 },
{ OV(463),   105 },
{ OV(506),   100 },
{ OV(549),   95 },
{ OV(594),   90 },
{ OV(638),   85 },
{ OV(681),   80 },
{ OV(722),   75 },
{ OV(762),   70 },
{ OV(799),   65 },
{ OV(833),   60 },
{ OV(863),   55 },
{ OV(890),   50 },
{ OV(914),   45 },
{ OV(934),   40 },
{ OV(951),   35 },
{ OV(966),   30 },
{ OV(978),   25 },
{ OV(988),   20 },
{ OV(996),   15 },
{ OV(1002),  10 },
{ OV(1007),   5 },
{ OV(1012),   0 },
{ OV(1015),  -5 },
{ OV(1017), -10 },
{ OV(1019), -15 },
{ OV(1020), -20 },
{ OV(1021), -25 },
{ OV(1022), -30 },
{ OV(1023), -35 },
{ OV(1023), -40 }
};
