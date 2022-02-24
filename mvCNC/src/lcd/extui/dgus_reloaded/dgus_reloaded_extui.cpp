/**
 * Modern Vintage CNC Firmware
 * Copyright (c) 2022 Edward Webber [https://github.com/Domush/mvCNC-Modern-Vintage-CNC-Firmware]

 * Based on Marlin and grbl.
 */
/**
 * lcd/extui/dgus_reloaded/dgus_reloaded_extui.cpp
 */

#include "../../../inc/mvCNCConfigPre.h"

#if ENABLED(DGUS_LCD_UI_RELOADED)

#include "../ui_api.h"
#include "DGUSScreenHandler.h"

namespace ExtUI {

  void onStartup() { dgus_screen_handler.Init(); }

  void onIdle() {
    static bool processing = false;

    // Prevent recursion
    if (!processing) {
      processing = true;
      dgus_screen_handler.Loop();
      processing = false;
    }
  }

  void onCNCKilled(FSTR_P const error, FSTR_P const component) {
    dgus_screen_handler.CNCKilled(error, component);
  }

  void onMediaInserted() { TERN_(SDSUPPORT, dgus_screen_handler.SDCardInserted()); }
  void onMediaError()    { TERN_(SDSUPPORT, dgus_screen_handler.SDCardError()); }
  void onMediaRemoved()  { TERN_(SDSUPPORT, dgus_screen_handler.SDCardRemoved()); }

  void onPlayTone(const uint16_t frequency, const uint16_t duration) {
    dgus_screen_handler.PlayTone(frequency, duration);
  }

  void onPrintTimerStarted() {
    dgus_screen_handler.CNCTimerStarted();
  }

  void onPrintTimerPaused() {
    dgus_screen_handler.CNCTimerPaused();
  }

  void onPrintTimerStopped() {
    dgus_screen_handler.CNCTimerStopped();
  }

  void onFilamentRunout(const extruder_t extruder) {
    dgus_screen_handler.FilamentRunout(extruder);
  }

  void onUserConfirmRequired(const char * const msg) {
    dgus_screen_handler.UserConfirmRequired(msg);
  }

  void onStatusChanged(const char * const msg) {
    dgus_screen_handler.SetStatusMessage(msg);
  }

  void onHomingStart() {}
  void onHomingComplete() {}
  void onPrintFinished() {}

  void onFactoryReset() {
    dgus_screen_handler.SettingsReset();
  }

  void onStoreSettings(char *buff) {
    dgus_screen_handler.StoreSettings(buff);
  }

  void onLoadSettings(const char *buff) {
    dgus_screen_handler.LoadSettings(buff);
  }

  void onPostprocessSettings() {}

  void onConfigurationStoreWritten(bool success) {
    dgus_screen_handler.ConfigurationStoreWritten(success);
  }

  void onConfigurationStoreRead(bool success) {
    dgus_screen_handler.ConfigurationStoreRead(success);
  }

  #if HAS_MESH
    void onMeshLevelingStart() {}

    void onMeshUpdate(const int8_t xpos, const int8_t ypos, const_float_t zval) {
      dgus_screen_handler.MeshUpdate(xpos, ypos);
    }

    void onMeshUpdate(const int8_t xpos, const int8_t ypos, const probe_state_t state) {
      if (state == G29_POINT_FINISH)
        dgus_screen_handler.MeshUpdate(xpos, ypos);
    }
  #endif

  #if ENABLED(POWER_LOSS_RECOVERY)
    void onPowerLossResume() {
      // Called on resume from power-loss
      dgus_screen_handler.PowerLossResume();
    }
  #endif

  #if HAS_PID_HEATING
    void onPidTuning(const result_t rst) {
      // Called for temperature PID tuning result
      dgus_screen_handler.PidTuning(rst);
    }
  #endif

  void onSteppersDisabled() {}
  void onSteppersEnabled()  {}
}

#endif // DGUS_LCD_UI_RELOADED
