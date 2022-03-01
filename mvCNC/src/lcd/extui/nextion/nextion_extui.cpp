/**
 * Modern Vintage CNC Firmware
 * Copyright (c) 2022 Edward Webber [https://github.com/Domush/mvCNC-Modern-Vintage-CNC-Firmware]

 * Based on Marlin and grbl.
 */
/**
 * lcd/extui/nextion_lcd.cpp
 *
 * Nextion TFT support for mvCNC
 */

#include "../../../inc/mvCNCConfigPre.h"

#if ENABLED(NEXTION_TFT)

#include "../ui_api.h"
#include "nextion_tft.h"

namespace ExtUI {

  void onStartup()                                   { nextion.Startup();  }
  void onIdle()                                      { nextion.IdleLoop(); }
  void onCNCKilled(FSTR_P const error, FSTR_P const component) { nextion.CNCKilled(error, component); }
  void onMediaInserted() {}
  void onMediaError()    {}
  void onMediaRemoved()  {}
  void onPlayTone(const uint16_t frequency, const uint16_t duration) {}
  void onPrintTimerStarted() {}
  void onPrintTimerPaused()  {}
  void onPrintTimerStopped() {}
  void onFilamentRunout(const atc_tool_t)            {}
  void onUserConfirmRequired(const char * const msg) { nextion.ConfirmationRequest(msg); }
  void onStatusChanged(const char * const msg)       { nextion.StatusChange(msg);        }

  void onHomingStart()    {}
  void onHomingComplete() {}
  void onPrintFinished()                             { nextion.CNCFinished(); }

  void onFactoryReset()   {}

  void onStoreSettings(char *buff) {
    // Called when saving to EEPROM (i.e. M500). If the ExtUI needs
    // permanent data to be stored, it can write up to eeprom_data_size bytes
    // into buff.

    // Example:
    //  static_assert(sizeof(myDataStruct) <= ExtUI::eeprom_data_size);
    //  memcpy(buff, &myDataStruct, sizeof(myDataStruct));
  }

  void onLoadSettings(const char *buff) {
    // Called while loading settings from EEPROM. If the ExtUI
    // needs to retrieve data, it should copy up to eeprom_data_size bytes
    // from buff

    // Example:
    //  static_assert(sizeof(myDataStruct) <= ExtUI::eeprom_data_size);
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

    void onMeshUpdate(const int8_t xpos, const int8_t ypos, const float zval) {
      // Called when any mesh points are updated
    }

    void onMeshUpdate(const int8_t xpos, const int8_t ypos, const ExtUI::probe_state_t state) {
      // Called to indicate a special condition
    }
  #endif

  #if ENABLED(POWER_LOSS_RECOVERY)
    void onPowerLossResume() {
      // Called on resume from power-loss
    }
  #endif

  #if HAS_PID_HEATING
    void onPidTuning(const result_t rst) {
      // Called for temperature PID tuning result
      nextion.PanelInfo(37);
    }
  #endif

  void onSteppersDisabled() {}
  void onSteppersEnabled()  {}
}

#endif // NEXTION_TFT
