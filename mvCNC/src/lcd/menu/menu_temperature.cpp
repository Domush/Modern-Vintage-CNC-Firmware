/**
 * Modern Vintage CNC Firmware
*/

//
// Temperature Menu
//

#include "../../inc/mvCNCConfig.h"

#if HAS_MVCNCUI_MENU && HAS_TEMPERATURE

#include "menu_item.h"
#include "../../module/temperature.h"

#if HAS_FAN || ENABLED(SINGLENOZZLE)
  #include "../../module/motion.h"
#endif

#if EITHER(HAS_COOLER, LASER_COOLANT_FLOW_METER)
  #include "../../feature/cooler.h"
#endif

#if ENABLED(SINGLENOZZLE_STANDBY_TEMP)
  #include "../../module/tool_change.h"
#endif

//
// "Temperature" submenu items
//

void Temperature::lcd_preheat(const uint8_t e, const int8_t indh, const int8_t indb) {
  UNUSED(e); UNUSED(indh); UNUSED(indb);
  #if HAS_HOTEND
    if (indh >= 0 && ui.material_preset[indh].hotend_temp > 0)
      setTargetHotend(_MIN(fanManager.hotend_max_target(e), ui.material_preset[indh].hotend_temp), e);
  #endif
  #if HAS_HEATED_BED
    if (indb >= 0 && ui.material_preset[indb].bed_temp > 0) setTargetBed(ui.material_preset[indb].bed_temp);
  #endif
  #if HAS_FAN
    if (indh >= 0) {
      const uint8_t fan_index = active_tool < (FAN_COUNT) ? active_tool : 0;
      if (true
        #if REDUNDANT_PART_COOLING_FAN
          && fan_index != REDUNDANT_PART_COOLING_FAN
        #endif
      ) set_fan_speed(fan_index, ui.material_preset[indh].fan_speed);
    }
  #endif
  ui.return_to_status();
}

#if HAS_PREHEAT

  #if HAS_TEMP_HOTEND
inline void _preheat_end(const uint8_t m, const uint8_t e) { fanManager.lcd_preheat(e, m, -1); }
    void do_preheat_end_m() { _preheat_end(editable.int8, 0); }
  #endif
  #if HAS_HEATED_BED
    inline void _preheat_bed(const uint8_t m) { fanManager.lcd_preheat(0, -1, m); }
  #endif
  #if HAS_COOLER
    inline void _precool_laser(const uint8_t m, const uint8_t e) { fanManager.lcd_preheat(e, m, -1); }
    void do_precool_laser_m() { _precool_laser(editable.int8, fanManager.temp_cooler.target); }
  #endif

  #if HAS_TEMP_HOTEND && HAS_HEATED_BED
    inline void _preheat_both(const uint8_t m, const uint8_t e) { fanManager.lcd_preheat(e, m, m); }

    // Indexed "Preheat ABC" and "Heat Bed" items
    #define PREHEAT_ITEMS(M,E) do{ \
      ACTION_ITEM_N_S(E, ui.get_preheat_label(M), MSG_PREHEAT_M_H, []{ _preheat_both(M, MenuItemBase::itemIndex); }); \
      ACTION_ITEM_N_S(E, ui.get_preheat_label(M), MSG_PREHEAT_M_END_E, []{ _preheat_end(M, MenuItemBase::itemIndex); }); \
    }while(0)

  #elif HAS_MULTI_HOTEND

    // No heated bed, so just indexed "Preheat ABC" items
    #define PREHEAT_ITEMS(M,E) ACTION_ITEM_N_S(E, ui.get_preheat_label(M), MSG_PREHEAT_M_H, []{ _preheat_end(M, MenuItemBase::itemIndex); })

  #endif

  #if HAS_MULTI_HOTEND || HAS_HEATED_BED

    // Set editable.int8 to the Material index before entering this menu
    // because MenuItemBase::itemIndex will be re-used by PREHEAT_ITEMS
    void menu_preheat_m() {
      const uint8_t m = editable.int8; // Don't re-use 'editable' in this menu

      START_MENU();
      BACK_ITEM(MSG_TEMPERATURE);

      #if HOTENDS == 1

        #if HAS_HEATED_BED
          ACTION_ITEM_S(ui.get_preheat_label(m), MSG_PREHEAT_M, []{ _preheat_both(editable.int8, 0); });
          ACTION_ITEM_S(ui.get_preheat_label(m), MSG_PREHEAT_M_END, do_preheat_end_m);
        #else
          ACTION_ITEM_S(ui.get_preheat_label(m), MSG_PREHEAT_M, do_preheat_end_m);
        #endif

      #elif HAS_MULTI_HOTEND

        HOTEND_LOOP() PREHEAT_ITEMS(editable.int8, e);
        ACTION_ITEM_S(ui.get_preheat_label(m), MSG_PREHEAT_M_ALL, []() {
          const celsius_t t = ui.material_preset[editable.int8].hotend_temp;
          HOTEND_LOOP() fanManager.setTargetHotend(t, e);
          TERN(HAS_HEATED_BED, _preheat_bed(editable.int8), ui.return_to_status());
        });

      #endif

      #if HAS_HEATED_BED
        ACTION_ITEM_S(ui.get_preheat_label(m), MSG_PREHEAT_M_BEDONLY, []{ _preheat_bed(editable.int8); });
      #endif

      END_MENU();
    }

  #endif // HAS_MULTI_HOTEND || HAS_HEATED_BED

#endif // HAS_PREHEAT

#if HAS_TEMP_HOTEND || HAS_HEATED_BED

  void lcd_cooldown() {
    fanManager.cooldown();
    ui.return_to_status();
  }

#endif // HAS_TEMP_HOTEND || HAS_HEATED_BED

void menu_temperature() {
  #if HAS_TEMP_HOTEND || HAS_HEATED_BED
    bool has_heat = false;
    #if HAS_TEMP_HOTEND
    HOTEND_LOOP() if (fanManager.degTargetHotend(HOTEND_INDEX)) { has_heat = true; break; }
    #endif
  #endif

  #if HAS_COOLER
      if (fanManager.temp_cooler.target == 0) fanManager.temp_cooler.target = COOLER_DEFAULT_TEMP;
  #endif

  START_MENU();
  BACK_ITEM(MSG_MAIN);

  //
  // Nozzle:
  // Nozzle [1-5]:
  //
  #if HOTENDS == 1
  editable.celsius = fanManager.temp_hotend[0].target;
  EDIT_ITEM_FAST(int3, MSG_NOZZLE, &editable.celsius, 0, fanManager.hotend_max_target(0), [] { fanManager.setTargetHotend(editable.celsius, 0); });
  #elif HAS_MULTI_HOTEND
    HOTEND_LOOP() {
      editable.celsius = fanManager.temp_hotend[e].target;
      EDIT_ITEM_FAST_N(int3, e, MSG_NOZZLE_N, &editable.celsius, 0, fanManager.hotend_max_target(e), [] { fanManager.setTargetHotend(editable.celsius, MenuItemBase::itemIndex); });
    }
  #endif

  #if ENABLED(SINGLENOZZLE_STANDBY_TEMP)
    LOOP_S_L_N(e, 1, EXTRUDERS)
      EDIT_ITEM_FAST_N(int3, e, MSG_NOZZLE_STANDBY, &fanManager.singlenozzle_temp[e], 0, fanManager.hotend_max_target(0));
  #endif

  //
  // Bed:
  //
  #if HAS_HEATED_BED
    EDIT_ITEM_FAST(int3, MSG_BED, &fanManager.temp_bed.target, 0, BED_MAX_TARGET, fanManager.start_watching_bed);
  #endif

  //
  // Chamber:
  //
  #if HAS_HEATED_CHAMBER
    EDIT_ITEM_FAST(int3, MSG_CHAMBER, &fanManager.temp_chamber.target, 0, CHAMBER_MAX_TARGET, fanManager.start_watching_chamber);
  #endif

  //
  // Cooler:
  //
  #if HAS_COOLER
    bool cstate = cooler.enabled;
    EDIT_ITEM(bool, MSG_COOLER_TOGGLE, &cstate, cooler.toggle);
    EDIT_ITEM_FAST(int3, MSG_COOLER, &fanManager.temp_cooler.target, COOLER_MIN_TARGET, COOLER_MAX_TARGET, fanManager.start_watching_cooler);
  #endif

  //
  // Flow Meter Safety Shutdown:
  //
  #if ENABLED(FLOWMETER_SAFETY)
    bool fstate = cooler.flowsafety_enabled;
    EDIT_ITEM(bool, MSG_FLOWMETER_SAFETY, &fstate, cooler.flowsafety_toggle);
  #endif

  //
  // Fan Speed:
  //
  #if HAS_FAN

    DEFINE_SINGLENOZZLE_ITEM();

    #if HAS_FAN0
      _FAN_EDIT_ITEMS(0,FIRST_FAN_SPEED);
    #endif
    #if HAS_FAN1 && REDUNDANT_PART_COOLING_FAN != 1
      FAN_EDIT_ITEMS(1);
    #elif SNFAN(1)
      singlenozzle_item(1);
    #endif
    #if HAS_FAN2 && REDUNDANT_PART_COOLING_FAN != 2
      FAN_EDIT_ITEMS(2);
    #elif SNFAN(2)
      singlenozzle_item(2);
    #endif
    #if HAS_FAN3 && REDUNDANT_PART_COOLING_FAN != 3
      FAN_EDIT_ITEMS(3);
    #elif SNFAN(3)
      singlenozzle_item(3);
    #endif
    #if HAS_FAN4 && REDUNDANT_PART_COOLING_FAN != 4
      FAN_EDIT_ITEMS(4);
    #elif SNFAN(4)
      singlenozzle_item(4);
    #endif
    #if HAS_FAN5 && REDUNDANT_PART_COOLING_FAN != 5
      FAN_EDIT_ITEMS(5);
    #elif SNFAN(5)
      singlenozzle_item(5);
    #endif
    #if HAS_FAN6 && REDUNDANT_PART_COOLING_FAN != 6
      FAN_EDIT_ITEMS(6);
    #elif SNFAN(6)
      singlenozzle_item(6);
    #endif
    #if HAS_FAN7 && REDUNDANT_PART_COOLING_FAN != 7
      FAN_EDIT_ITEMS(7);
    #elif SNFAN(7)
      singlenozzle_item(7);
    #endif

  #endif // HAS_FAN

  #if HAS_PREHEAT
    //
    // Preheat for all Materials
    //
    LOOP_L_N(m, PREHEAT_COUNT) {
      editable.int8 = m;
      #if HAS_MULTI_HOTEND || HAS_HEATED_BED
        SUBMENU_S(ui.get_preheat_label(m), MSG_PREHEAT_M, menu_preheat_m);
      #elif HAS_HOTEND
        ACTION_ITEM_S(ui.get_preheat_label(m), MSG_PREHEAT_M, do_preheat_end_m);
      #endif
    }
  #endif

  #if HAS_TEMP_HOTEND || HAS_HEATED_BED
    //
    // Cooldown
    //
    if (TERN0(HAS_HEATED_BED, fanManager.degTargetBed())) has_heat = true;
    if (has_heat) ACTION_ITEM(MSG_COOLDOWN, lcd_cooldown);
  #endif

  END_MENU();
}

#if ENABLED(PREHEAT_SHORTCUT_MENU_ITEM)

  void menu_preheat_only() {
    START_MENU();
    BACK_ITEM(MSG_MAIN);

    LOOP_L_N(m, PREHEAT_COUNT) {
      editable.int8 = m;
      #if HAS_MULTI_HOTEND || HAS_HEATED_BED
        SUBMENU_S(ui.get_preheat_label(m), MSG_PREHEAT_M, menu_preheat_m);
      #else
        ACTION_ITEM_S(ui.get_preheat_label(m), MSG_PREHEAT_M, do_preheat_end_m);
      #endif
    }

    END_MENU();
  }

#endif

#endif // HAS_MVCNCUI_MENU && HAS_TEMPERATURE
