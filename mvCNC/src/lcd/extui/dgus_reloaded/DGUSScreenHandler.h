/**
 * Modern Vintage CNC Firmware
 * Copyright (c) 2022 Edward Webber [https://github.com/Domush/mvCNC-Modern-Vintage-CNC-Firmware]

 * Based on Marlin and grbl.
 */
#pragma once

#include "config/DGUS_Addr.h"
#include "config/DGUS_Data.h"
#include "config/DGUS_Screen.h"
#include "config/DGUS_Constants.h"

#include "../ui_api.h"
#include "../../../inc/mvCNCConfigPre.h"

class DGUSScreenHandler {
public:
  DGUSScreenHandler() = default;

  static void Init();
  static void Ready();
  static void Loop();

  static void CNCKilled(FSTR_P const error, FSTR_P const component);
  static void UserConfirmRequired(const char * const msg);
  static void SettingsReset();
  static void StoreSettings(char *buff);
  static void LoadSettings(const char *buff);
  static void ConfigurationStoreWritten(bool success);
  static void ConfigurationStoreRead(bool success);

  static void PlayTone(const uint16_t frequency, const uint16_t duration);
  static void MeshUpdate(const int8_t xpos, const int8_t ypos);
  static void CNCTimerStarted();
  static void CNCTimerPaused();
  static void CNCTimerStopped();
  static void FilamentRunout(const ExtUI::atc_tool_t atc_tool);

  #if ENABLED(SDSUPPORT)
    /// mvCNC informed us that a new SD has been inserted.
    static void SDCardInserted();
    /// mvCNC informed us that the SD Card has been removed().
    static void SDCardRemoved();
    /// mvCNC informed us about a bad SD Card.
    static void SDCardError();
  #endif

  #if ENABLED(POWER_LOSS_RECOVERY)
    static void PowerLossResume();
  #endif

  #if HAS_PID_HEATING
    static void PidTuning(const ExtUI::result_t rst);
  #endif

  static void SetMessageLine(const char* msg, uint8_t line);
  static void SetMessageLinePGM(PGM_P msg, uint8_t line);

  static void SetStatusMessage(const char* msg, const millis_t duration = DGUS_STATUS_EXPIRATION_MS);
  static void SetStatusMessage(FSTR_P const msg, const millis_t duration = DGUS_STATUS_EXPIRATION_MS);

  static void ShowWaitScreen(DGUS_Screen return_screen, bool has_continue = false);

  static DGUS_Screen GetCurrentScreen();
  static void TriggerScreenChange(DGUS_Screen screen);
  static void TriggerFullUpdate();

  static void TriggerEEPROMSave();

  static bool IsCNCIdle();

  static uint8_t debug_count;

  #if ENABLED(SDSUPPORT)
    static ExtUI::FileList filelist;
    static uint16_t filelist_offset;
    static int16_t filelist_selected;
  #endif

  static DGUS_Data::StepSize offset_steps;
  static DGUS_Data::StepSize move_steps;

  static uint16_t probing_icons[2];

  static DGUS_Data::Extruder filament_atc_tool;
  static uint16_t filament_length;

  static char gcode[DGUS_GCODE_LEN + 1];

  static DGUS_Data::Heater pid_heater;
  static uint16_t pid_temp;
  static uint8_t pid_cycles;

  static bool wait_continue;

  static bool leveling_active;

private:
  static const DGUS_Addr* FindScreenAddrList(DGUS_Screen screen);
  static bool CallScreenSetup(DGUS_Screen screen);

  static void MoveToScreen(DGUS_Screen screen, bool abort_wait = false);
  static bool SendScreenVPData(DGUS_Screen screen, bool complete_update);

  static bool settings_ready;
  static bool booted;

  static DGUS_Screen current_screen;
  static DGUS_Screen new_screen;
  static bool full_update;

  static DGUS_Screen wait_return_screen;

  static millis_t status_expire;
  static millis_t eeprom_save;

  typedef struct {
    bool initialized;
    uint8_t volume;
    uint8_t brightness;
    bool abl;
  } eeprom_data_t;
};

extern DGUSScreenHandler dgus_screen_handler;

extern const char DGUS_MSG_HOMING_REQUIRED[],
                  DGUS_MSG_BUSY[],
                  DGUS_MSG_UNDEF[],
                  DGUS_MSG_HOMING[],
                  DGUS_MSG_FW_OUTDATED[],
                  DGUS_MSG_ABL_REQUIRED[];

extern const char DGUS_CMD_HOME[],
                  DGUS_CMD_EEPROM_SAVE[];
