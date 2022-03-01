/**
 * Modern Vintage CNC Firmware
*/

/**
 * lcd/extui/dgus/dgus_extui.cpp
 */

#include "../../../inc/mvCNCConfigPre.h"

#if HAS_DGUS_LCD_CLASSIC

#include "../ui_api.h"
#include "DGUSDisplay.h"
#include "DGUSDisplayDef.h"
#include "DGUSScreenHandler.h"

namespace ExtUI {

  void onStartup() {
    dgusdisplay.InitDisplay();
    ScreenHandler.UpdateScreenVPData();
  }

  void onIdle() { ScreenHandler.loop(); }

  void onCNCKilled(FSTR_P const error, FSTR_P const) {
    ScreenHandler.sendinfoscreen(GET_TEXT_F(MSG_HALTED), error, FPSTR(NUL_STR), GET_TEXT_F(MSG_PLEASE_RESET), true, true, true, true);
    ScreenHandler.GotoScreen(DGUSLCD_SCREEN_KILL);
    while (!ScreenHandler.loop());  // Wait while anything is left to be sent
  }

  void onMediaInserted() { TERN_(SDSUPPORT, ScreenHandler.SDCardInserted()); }
  void onMediaError()    { TERN_(SDSUPPORT, ScreenHandler.SDCardError()); }
  void onMediaRemoved()  { TERN_(SDSUPPORT, ScreenHandler.SDCardRemoved()); }

  void onPlayTone(const uint16_t frequency, const uint16_t duration) {}
  void onPrintTimerStarted() {}
  void onPrintTimerPaused() {}
  void onPrintTimerStopped() {}
  void onFilamentRunout(const atc_tool_t atc_tool) {}

  void onUserConfirmRequired(const char * const msg) {
    if (msg) {
      ScreenHandler.sendinfoscreen(F("Please confirm."), nullptr, msg, nullptr, true, true, false, true);
      ScreenHandler.SetupConfirmAction(setUserConfirmed);
      ScreenHandler.GotoScreen(DGUSLCD_SCREEN_POPUP);
    }
    else if (ScreenHandler.getCurrentScreen() == DGUSLCD_SCREEN_POPUP) {
      ScreenHandler.SetupConfirmAction(nullptr);
      ScreenHandler.PopToOldScreen();
    }
  }

  void onStatusChanged(const char * const msg) { ScreenHandler.setstatusmessage(msg); }

  void onHomingStart() {}
  void onHomingComplete() {}
  void onPrintFinished() {}

  void onFactoryReset() {}

  void onStoreSettings(char *buff) {
    // Called when saving to EEPROM (i.e. M500). If the ExtUI needs
    // permanent data to be stored, it can write up to eeprom_data_size bytes
    // into buff.

    // Example:
    //  static_assert(sizeof(myDataStruct) <= eeprom_data_size);
    //  memcpy(buff, &myDataStruct, sizeof(myDataStruct));
  }

  void onLoadSettings(const char *buff) {
    // Called while loading settings from EEPROM. If the ExtUI
    // needs to retrieve data, it should copy up to eeprom_data_size bytes
    // from buff

    // Example:
    //  static_assert(sizeof(myDataStruct) <= eeprom_data_size);
    //  memcpy(&myDataStruct, buff, sizeof(myDataStruct));
  }

  void onPostprocessSettings() {
    // Called after loading or resetting stored settings
  }

  void onConfigurationStoreWritten(bool success) {
    // Called after the entire EEPROM has been written,
    // whether successful or not.
  }

  void onConfigurationStoreRead(bool success) {
    // Called after the entire EEPROM has been read,
    // whether successful or not.
  }

  #if HAS_MESH
    void onMeshLevelingStart() {}

    void onMeshUpdate(const int8_t xpos, const int8_t ypos, const_float_t zval) {
      // Called when any mesh points are updated
    }

    void onMeshUpdate(const int8_t xpos, const int8_t ypos, const probe_state_t state) {
      // Called to indicate a special condition
    }
  #endif

  #if ENABLED(POWER_LOSS_RECOVERY)
    void onPowerLossResume() {
      // Called on resume from power-loss
      IF_DISABLED(DGUS_LCD_UI_MKS, ScreenHandler.GotoScreen(DGUSLCD_SCREEN_POWER_LOSS));
    }
  #endif

  #if HAS_PID_HEATING
    void onPidTuning(const result_t rst) {
      // Called for temperature PID tuning result
      switch (rst) {
        case PID_STARTED:
          ScreenHandler.setstatusmessagePGM(GET_TEXT(MSG_PID_AUTOTUNE));
          break;
        case PID_BAD_EXTRUDER_NUM:
          ScreenHandler.setstatusmessagePGM(GET_TEXT(MSG_PID_BAD_EXTRUDER_NUM));
          break;
        case PID_TEMP_TOO_HIGH:
          ScreenHandler.setstatusmessagePGM(GET_TEXT(MSG_PID_TEMP_TOO_HIGH));
          break;
        case PID_TUNING_TIMEOUT:
          ScreenHandler.setstatusmessagePGM(GET_TEXT(MSG_PID_TIMEOUT));
          break;
        case PID_DONE:
          ScreenHandler.setstatusmessagePGM(GET_TEXT(MSG_PID_AUTOTUNE_DONE));
          break;
      }
      ScreenHandler.GotoScreen(DGUSLCD_SCREEN_MAIN);
    }
  #endif

  void onSteppersDisabled() {}
  void onSteppersEnabled()  {}
}

#endif // HAS_DGUS_LCD_CLASSIC
