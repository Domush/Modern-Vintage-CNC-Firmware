/**
 * Modern Vintage CNC Firmware
*/
#pragma once

/**
 * planner_bezier.h
 *
 * Compute and buffer movement commands for Bézier curves
 */

#include "../core/types.h"

void cubic_b_spline(
  const xyze_pos_t &position,       // current position
  const xyze_pos_t &target,         // target position
  const xy_pos_t (&offsets)[2],     // a pair of offsets
  const_feedRate_t scaled_fr_mm_s,  // mm/s scaled by feedrate %
  const uint8_t atc_tool
);
