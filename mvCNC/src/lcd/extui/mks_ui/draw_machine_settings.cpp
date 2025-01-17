/**
 * Modern Vintage CNC Firmware
*/

#include "../../../inc/mvCNCConfigPre.h"

#if HAS_TFT_LVGL_UI

#include "draw_ui.h"
#include <lv_conf.h>

#include "../../../inc/mvCNCConfig.h"

extern lv_group_t *g;
static lv_obj_t *scr;

enum {
  ID_MACHINE_RETURN = 1,
  ID_MACHINE_ACCELERATION,
  ID_MACHINE_FEEDRATE,
  ID_MACHINE_JERK
};

static void event_handler(lv_obj_t *obj, lv_event_t event) {
  if (event != LV_EVENT_RELEASED) return;
  clear_cur_ui();
  switch (obj->mks_obj_id) {
    case ID_MACHINE_RETURN:       draw_return_ui(); break;
    case ID_MACHINE_ACCELERATION: lv_draw_acceleration_settings(); break;
    case ID_MACHINE_FEEDRATE:     lv_draw_max_feedrate_settings(); break;
    #if HAS_CLASSIC_JERK
      case ID_MACHINE_JERK:       lv_draw_jerk_settings(); break;
    #endif
  }
}

void lv_draw_machine_settings() {
  scr = lv_screen_create(MACHINE_SETTINGS_UI, machine_menu.MachineConfigTitle);
  lv_coord_t y = PARA_UI_POS_Y;
  lv_screen_menu_item(scr, machine_menu.AccelerationConf, PARA_UI_POS_X, y, event_handler, ID_MACHINE_ACCELERATION, 0);
  y += PARA_UI_POS_Y;
  lv_screen_menu_item(scr, machine_menu.MaxFeedRateConf, PARA_UI_POS_X, y, event_handler, ID_MACHINE_FEEDRATE, 1);
  #if HAS_CLASSIC_JERK
    y += PARA_UI_POS_Y;
    lv_screen_menu_item(scr, machine_menu.JerkConf, PARA_UI_POS_X, y, event_handler, ID_MACHINE_JERK, 2);
  #endif
  lv_big_button_create(scr, "F:/bmp_back70x40.bin", common_menu.text_back, PARA_UI_BACK_POS_X + 10, PARA_UI_BACK_POS_Y, event_handler, ID_MACHINE_RETURN, true);
}

void lv_clear_machine_settings() {
  #if HAS_ROTARY_ENCODER
    if (gCfgItems.encoder_enable) lv_group_remove_all_objs(g);
  #endif
  lv_obj_del(scr);
}

#endif // HAS_TFT_LVGL_UI
