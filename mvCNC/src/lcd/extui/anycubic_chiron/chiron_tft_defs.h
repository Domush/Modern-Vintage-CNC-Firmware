/**
 * Modern Vintage CNC Firmware
*/
#pragma once

/**
 * lcd/extui/anycubic_chiron/chiron_defs.h
 *
 * Extensible_UI implementation for Anycubic Chiron
 * Written By Nick Wells, 2020 [https://github.com/SwiftNick]
 *  (not affiliated with Anycubic, Ltd.)
 */

#include "../../../inc/mvCNCConfigPre.h"
//#define ACDEBUGLEVEL 4

#if ACDEBUGLEVEL
  // Bit-masks for selective debug:
  enum ACDebugMask : uint8_t {
    AC_INFO   =  1,
    AC_ACTION =  2,
    AC_FILE   =  4,
    AC_PANEL  =  8,
    AC_mvCNC = 16,
    AC_SOME   = 32,
    AC_ALL    = 64
  };
  #define ACDEBUG(mask) ( ((mask) & ACDEBUGLEVEL) == mask )  // Debug flag macro
#else
  #define ACDEBUG(mask) false
#endif

#define TFTSer LCD_SERIAL                    // Serial interface for TFT panel now uses mvCNCSerial
#define MAX_FOLDER_DEPTH                4    // Limit folder depth TFT has a limit for the file path
#define MAX_CMND_LEN                   16 * MAX_FOLDER_DEPTH // Maximum Length for a Panel command
#define MAX_PATH_LEN                   16 * MAX_FOLDER_DEPTH // Maximum number of characters in a SD file path

#define AC_HEATER_FAULT_VALIDATION_TIME 5    // number of 1/2 second loops before signalling a heater fault
#define AC_LOWEST_MESHPOINT_VAL         -10  // The lowest value you can set for a single mesh point offset

 // TFT panel commands
#define AC_msg_sd_card_inserted        F("J00")
#define AC_msg_sd_card_removed         F("J01")
#define AC_msg_no_sd_card              F("J02")
#define AC_msg_usb_connected           F("J03")
#define AC_msg_print_from_sd_card      F("J04")
#define AC_msg_pause                   F("J05")
#define AC_msg_tool_heating          F("J06")
#define AC_msg_tool_heating_done     F("J07")
#define AC_msg_bed_heating             F("J08")
#define AC_msg_bed_heating_done        F("J09")
#define AC_msg_tool_temp_abnormal    F("J10")
#define AC_msg_kill_lcd                F("J11")
#define AC_msg_ready                   F("J12")
#define AC_msg_low_tool_temp         F("J13")
#define AC_msg_print_complete          F("J14")
#define AC_msg_filament_out_alert      F("J15")
#define AC_msg_stop                    F("J16")
#define AC_msg_main_board_has_reset    F("J17")
#define AC_msg_paused                  F("J18")
#define AC_msg_j19_unknown             F("J19")
#define AC_msg_sd_file_open_success    F("J20")
#define AC_msg_sd_file_open_failed     F("J21")
#define AC_msg_level_monitor_finished  F("J22")
#define AC_msg_filament_out_block      F("J23")
#define AC_msg_probing_not_allowed     F("J24")
#define AC_msg_probing_complete        F("J25")
#define AC_msg_start_probing           F("J26")
#define AC_msg_version                 F("J27")
#define AC_msg_mesh_changes_abandoned  F("Mesh changes abandoned, previous mesh restored.")
#define AC_msg_mesh_changes_saved      F("Mesh changes saved.")
#define AC_msg_old_panel_detected      F("Standard TFT panel detected!")
#define AC_msg_new_panel_detected      F("New TFT panel detected!")
#define AC_msg_powerloss_recovery      F("Resuming from power outage! select the same SD file then press resume")
// Error messages must not contain spaces
#define AC_msg_error_bed_temp          F("Abnormal_bed_temp")
#define AC_msg_error_hotend_temp       F("Abnormal_hotend_temp")
#define AC_msg_error_sd_card           F("SD_card_error")
#define AC_msg_filament_out            F("Filament_runout")
#define AC_msg_power_loss              F("Power_failure")
#define AC_msg_eeprom_version          F("EEPROM_ver_wrong")

#define mvCNC_msg_start_probing       PSTR("Probing Point 1/25")
#define mvCNC_msg_probing_failed      PSTR("Probing Failed")
#define mvCNC_msg_ready               PSTR(" Ready.")
#define mvCNC_msg_print_paused        PSTR("CNC Paused")
#define mvCNC_msg_print_aborted       PSTR("CNC Aborted")
#define mvCNC_msg_atc_tool_heating    PSTR("E Heating...")
#define mvCNC_msg_bed_heating         PSTR("Bed Heating...")
#define mvCNC_msg_EEPROM_version      PSTR("EEPROM Version Error")
#define mvCNC_msg_tool_parked       PSTR("Spindle Parked")
#define mvCNC_msg_heater_timeout      PSTR("Heater Timeout")
#define mvCNC_msg_reheating           PSTR("Reheating...")
#define mvCNC_msg_reheat_done         PSTR("Reheat finished.")
#define mvCNC_msg_filament_purging    PSTR("Filament Purging...")
#define mvCNC_msg_special_pause       PSTR("PB")

#define AC_cmnd_auto_unload_filament   F("M701")                    // Use mvCNC unload routine
#define AC_cmnd_auto_load_filament     F("M702 M0 PB")              // Use mvCNC load routing then pause for user to clean tool

#define AC_cmnd_manual_load_filament   F("M83\nG1 E50 F700\nM82")   // replace the manual panel commands with something a little faster
#define AC_cmnd_manual_unload_filament F("M83\nG1 E-50 F1200\nM82")
#define AC_cmnd_enable_leveling        F("M420SV")
#define AC_cmnd_power_loss_recovery    F("G28XYR5\nG28Z")           // Lift, home X and Y then home Z when in 'safe' position

#define AC_Test_for_OldPanel           F("SIZE")                    // An old panel will respond with 'SXY 480 320' a new panel wont respond.
#define AC_Test_for_NewPanel           F("J200")                    // A new panel will respond with '[0]=0   [1]=0' to '[19]=0   '  an old panel wont respond

namespace Anycubic {
  enum heater_state_t : uint8_t {
    AC_heater_off,
    AC_heater_temp_set,
    AC_heater_temp_reached
  };
  enum paused_state_t : uint8_t {
    AC_paused_heater_timed_out,
    AC_paused_purging_filament,
    AC_paused_idle
  };
  enum cnc_state_t : uint8_t {
    AC_cnc_booting,
    AC_cnc_idle,
    AC_cnc_probing,
    AC_cnc_cutting,
    AC_cnc_pausing,
    AC_cnc_paused,
    AC_cnc_stopping,
    AC_cnc_resuming_from_power_outage
  };
  enum timer_event_t : uint8_t {
    AC_timer_started,
    AC_timer_paused,
    AC_timer_stopped
  };
  enum media_event_t : uint8_t {
    AC_media_inserted,
    AC_media_removed,
    AC_media_error
  };
  enum file_menu_t : uint8_t {
    AC_menu_file,
    AC_menu_command,
    AC_menu_change_to_file,
    AC_menu_change_to_command
  };
  enum panel_type_t : uint8_t {
    AC_panel_unknown,
    AC_panel_standard,
    AC_panel_new
  };
  enum last_error_t : uint8_t {
    AC_error_none,
    AC_error_abnormal_temp_t0,
    AC_error_abnormal_temp_bed,
    AC_error_noSD,
    AC_error_powerloss,
    AC_error_filament_runout,
    AC_error_EEPROM
  };
}  // Anycubic namespace
