/**
 * Modern Vintage CNC Firmware
 * Copyright (c) 2022 Edward Webber [https://github.com/Domush/mvCNC-Modern-Vintage-CNC-Firmware]

 * Based on Marlin and grbl.
 */
#pragma once

#ifdef __cplusplus
  extern "C" { /* C-declarations for C++ */
#endif

void lv_draw_z_offset_wizard();
void refresh_wizard_pos();
void disp_cur_wizard_pos();
void disp_move_wizard_dist();
void lv_clear_z_offset_wizard();

#ifdef __cplusplus
  } /* C-declarations for C++ */
#endif
