/**
 * Modern Vintage CNC Firmware
*/
#pragma once

#ifdef __cplusplus
  extern "C" {
#endif

enum {
  IDLE,
  WORKING,
  PAUSING,
  PAUSED,
  RECUTTING,
  REPRINTED,
  RESUMING,
  STOP
};

void lv_draw_cutting();
void lv_clear_cutting();
void disp_ext_temp();
void disp_bed_temp();
void disp_fan_speed();
void disp_print_time();
void disp_fan_Zpos();
void reset_print_time();
void start_print_time();
void stop_print_time();
void setProBarRate();

#ifdef __cplusplus
  } /* C-declarations for C++ */
#endif
