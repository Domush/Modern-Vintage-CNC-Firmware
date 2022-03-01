/**
 * Modern Vintage CNC Firmware
*/

/**
 * lcd/extui/anycubic_i3mega/anycubic_extui.cpp
 */

#include "../../../inc/mvCNCConfigPre.h"

#if ENABLED(ANYCUBIC_LCD_I3MEGA)

#include "anycubic_i3mega_lcd.h"
#include "../ui_api.h"

#include <Arduino.h>    // for the ::tone() call

namespace ExtUI {

  void onStartup()        { AnycubicTFT.OnSetup(); }
  void onIdle()           { AnycubicTFT.OnCommandScan(); }
  void onCNCKilled(FSTR_P const error, FSTR_P const component) { AnycubicTFT.OnKillTFT(); }
  void onMediaInserted()  { AnycubicTFT.OnSDCardStateChange(true); }
  void onMediaError()     { AnycubicTFT.OnSDCardError(); }
  void onMediaRemoved()   { AnycubicTFT.OnSDCardStateChange(false); }
  void onPlayTone(const uint16_t frequency, const uint16_t duration) {
    #if ENABLED(SPEAKER)
      ::tone(BEEPER_PIN, frequency, duration);
    #endif
  }
  void onPrintTimerStarted()  { AnycubicTFT.OnPrintTimerStarted(); }
  void onPrintTimerPaused()   { AnycubicTFT.OnPrintTimerPaused(); }
  void onPrintTimerStopped()  { AnycubicTFT.OnPrintTimerStopped(); }
  void onFilamentRunout(const atc_tool_t atc_tool)   { AnycubicTFT.OnFilamentRunout(); }
  void onUserConfirmRequired(const char * const msg) { AnycubicTFT.OnUserConfirmRequired(msg); }
  void onStatusChanged(const char * const msg) {}

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

    void onMeshUpdate(const int8_t xpos, const int8_t ypos, probe_state_t state) {
      // Called when any mesh points are updated
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
    }
  #endif

  void onSteppersDisabled() {}
  void onSteppersEnabled()  {}
}

#endif // ANYCUBIC_LCD_I3MEGA
