/**
 * Modern Vintage CNC Firmware
*/

#include "../../inc/mvCNCConfig.h"

#if BOTH(HAS_MVCNCUI_MENU, MMU2_MENUS)

#include "../../mvCNCCore.h"
#include "../../feature/mmu/mmu2.h"
#include "menu_mmu2.h"
#include "menu_item.h"

//
// Change Bit
//

inline void action_mmu2_load_filament_to_tool(const uint8_t tool) {
  ui.reset_status();
  ui.return_to_status();
  ui.status_printf(0, GET_TEXT_F(MSG_MMU2_LOADING_FILAMENT), int(tool + 1));
  if (mmu2.load_filament_to_tool(tool)) ui.reset_status();
  ui.return_to_status();
}

void _mmu2_change_bit(uint8_t index) {
  ui.return_to_status();
  ui.status_printf(0, GET_TEXT_F(MSG_MMU2_LOADING_FILAMENT), int(index + 1));
  mmu2.change_bit(index);
  ui.reset_status();
}
void action_mmu2_load_all() {
  LOOP_L_N(i, ATC_TOOLS) _mmu2_change_bit(i);
  ui.return_to_status();
}

void menu_mmu2_change_bit() {
  START_MENU();
  BACK_ITEM(MSG_MMU2_MENU);
  ACTION_ITEM(MSG_MMU2_ALL, action_mmu2_load_all);
  LOOP_L_N(i, ATC_TOOLS) ACTION_ITEM_N(i, MSG_MMU2_FILAMENT_N, []{ _mmu2_change_bit(MenuItemBase::itemIndex); });
  END_MENU();
}

void menu_mmu2_load_to_tool() {
  START_MENU();
  BACK_ITEM(MSG_MMU2_MENU);
  LOOP_L_N(i, ATC_TOOLS) ACTION_ITEM_N(i, MSG_MMU2_FILAMENT_N, []{ action_mmu2_load_filament_to_tool(MenuItemBase::itemIndex); });
  END_MENU();
}

//
// Eject Filament
//

void _mmu2_eject_filament(uint8_t index) {
  ui.reset_status();
  ui.return_to_status();
  ui.status_printf(0, GET_TEXT_F(MSG_MMU2_EJECTING_FILAMENT), int(index + 1));
  if (mmu2.eject_filament(index, true)) ui.reset_status();
}

void action_mmu2_unchange_bit() {
  ui.reset_status();
  ui.return_to_status();
  LCD_MESSAGE(MSG_MMU2_UNLOADING_FILAMENT);
  idle();
  if (mmu2.unload()) ui.reset_status();
}

void menu_mmu2_eject_filament() {
  START_MENU();
  BACK_ITEM(MSG_MMU2_MENU);
  LOOP_L_N(i, ATC_TOOLS) ACTION_ITEM_N(i, MSG_MMU2_FILAMENT_N, []{ _mmu2_eject_filament(MenuItemBase::itemIndex); });
  END_MENU();
}

//
// MMU2 Menu
//

void action_mmu2_reset() {
  mmu2.init();
  ui.reset_status();
}

void menu_mmu2() {
  START_MENU();
  BACK_ITEM(MSG_MAIN);
  SUBMENU(MSG_MMU2_LOAD_FILAMENT, menu_mmu2_load_filament);
  SUBMENU(MSG_MMU2_LOAD_TO_NOZZLE, menu_mmu2_load_to_tool);
  SUBMENU(MSG_MMU2_EJECT_FILAMENT, menu_mmu2_eject_filament);
  ACTION_ITEM(MSG_MMU2_UNLOAD_FILAMENT, action_mmu2_unload_filament);
  ACTION_ITEM(MSG_MMU2_RESET, action_mmu2_reset);
  END_MENU();
}

//
// T* Choose Filament
//

uint8_t feeder_index;
bool wait_for_mmu_menu;

inline void action_mmu2_chosen(const uint8_t index) {
  feeder_index = index;
  wait_for_mmu_menu = false;
}

void menu_mmu2_choose_filament() {
  START_MENU();
  #if LCD_HEIGHT > 2
    STATIC_ITEM(MSG_MMU2_CHOOSE_FILAMENT_HEADER, SS_DEFAULT|SS_INVERT);
  #endif
  LOOP_L_N(i, ATC_TOOLS) ACTION_ITEM_N(i, MSG_MMU2_FILAMENT_N, []{ action_mmu2_chosen(MenuItemBase::itemIndex); });
  END_MENU();
}

//
// MMU2 Filament Runout
//

void menu_mmu2_pause() {
  feeder_index = mmu2.get_current_tool();
  START_MENU();
  #if LCD_HEIGHT > 2
    STATIC_ITEM(MSG_FILAMENT_CHANGE_HEADER, SS_DEFAULT|SS_INVERT);
  #endif
  ACTION_ITEM(MSG_MMU2_RESUME, []{ wait_for_mmu_menu = false; });
  ACTION_ITEM(MSG_MMU2_UNLOAD_FILAMENT, []{ mmu2.unload(); });
  ACTION_ITEM(MSG_MMU2_LOAD_FILAMENT, []{ mmu2.change_bit(feeder_index); });
  ACTION_ITEM(MSG_MMU2_LOAD_TO_NOZZLE, []{ mmu2.load_filament_to_tool(feeder_index); });
  END_MENU();
}

void mmu2_M600() {
  ui.defer_status_screen();
  ui.goto_screen(menu_mmu2_pause);
  wait_for_mmu_menu = true;
  while (wait_for_mmu_menu) idle();
}

uint8_t mmu2_choose_filament() {
  ui.defer_status_screen();
  ui.goto_screen(menu_mmu2_choose_filament);
  wait_for_mmu_menu = true;
  while (wait_for_mmu_menu) idle();
  ui.return_to_status();
  return feeder_index;
}

#endif // HAS_MVCNCUI_MENU && MMU2_MENUS
