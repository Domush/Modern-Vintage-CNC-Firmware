/**
 * Modern Vintage CNC Firmware
 * Copyright (c) 2022 Edward Webber [https://github.com/Domush/mvCNC-Modern-Vintage-CNC-Firmware]

 * Based on Marlin and grbl.
 */
#include "../../../inc/mvCNCConfigPre.h"

#if BOTH(HAS_TFT_LVGL_UI, MULTI_VOLUME)

#include "draw_ui.h"
#include <lv_conf.h>

#include "../../../inc/mvCNCConfig.h"
#include "../../../sd/cardreader.h"

extern lv_group_t *g;
static lv_obj_t *scr;

enum {
  ID_T_USB_DISK = 1,
  ID_T_SD_DISK,
  ID_T_RETURN
};

#if ENABLED(MKS_TEST)
  extern uint8_t current_disp_ui;
#endif

static void event_handler(lv_obj_t *obj, lv_event_t event) {
  if (event != LV_EVENT_RELEASED) return;
  lv_clear_media_select();
  switch (obj->mks_obj_id) {
    case ID_T_USB_DISK: card.changeMedia(&card.media_driver_usbFlash); break;
    case ID_T_SD_DISK:  card.changeMedia(&card.media_driver_sdcard); break;
    case ID_T_RETURN:
      TERN_(MKS_TEST, current_disp_ui = 1);
      lv_draw_ready_print();
      return;
  }
  lv_draw_print_file();
}

void lv_draw_media_select() {
  scr = lv_screen_create(MEDIA_SELECT_UI);
  lv_big_button_create(scr, "F:/bmp_sd.bin", media_select_menu.sd_disk, INTERVAL_V, titleHeight, event_handler, ID_T_SD_DISK);
  lv_big_button_create(scr, "F:/bmp_usb_disk.bin", media_select_menu.usb_disk, BTN_X_PIXEL + INTERVAL_V * 2, titleHeight, event_handler, ID_T_USB_DISK);
  lv_big_button_create(scr, "F:/bmp_return.bin", common_menu.text_back, BTN_X_PIXEL * 3 + INTERVAL_V * 4, BTN_Y_PIXEL + INTERVAL_H + titleHeight, event_handler, ID_T_RETURN);
}

void lv_clear_media_select() {
  #if HAS_ROTARY_ENCODER
    if (gCfgItems.encoder_enable) lv_group_remove_all_objs(g);
  #endif
  lv_obj_del(scr);
}

#endif // HAS_TFT_LVGL_UI
