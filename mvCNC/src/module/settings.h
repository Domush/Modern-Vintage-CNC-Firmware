/**
 * Modern Vintage CNC Firmware
*/
#pragma once

//
// settings.cpp - Settings and EEPROM storage
//

#include "../inc/mvCNCConfig.h"

#if ENABLED(EEPROM_SETTINGS)
  #include "../HAL/shared/eeprom_api.h"
#endif

class mvCNCSettings {
  public:
    static uint16_t datasize();

    static void reset();
    static bool save();    // Return 'true' if data was saved

    FORCE_INLINE static bool init_eeprom() {
      reset();
      #if ENABLED(EEPROM_SETTINGS)
        const bool success = save();
        if (TERN0(EEPROM_CHITCHAT, success)) report();
        return success;
      #else
        return true;
      #endif
    }

    #if ENABLED(SD_FIRMWARE_UPDATE)
      static bool sd_update_status();                       // True if the SD-Firmware-Update EEPROM flag is set
      static bool set_sd_update_status(const bool enable);  // Return 'true' after EEPROM is set (-> always true)
    #endif

    #if ENABLED(EEPROM_SETTINGS)

      static bool load();      // Return 'true' if data was loaded ok
      static bool validate();  // Return 'true' if EEPROM data is ok

      static void first_load() {
        static bool loaded = false;
        if (!loaded && load()) loaded = true;
      }

    #else // !EEPROM_SETTINGS

      FORCE_INLINE
      static bool load() { reset(); report(); return true; }
      FORCE_INLINE
      static void first_load() { (void)load(); }

    #endif // !EEPROM_SETTINGS

    #if DISABLED(DISABLE_M503)
      static void report(const bool forReplay=false);
    #else
      FORCE_INLINE
      static void report(const bool=false) {}
    #endif

  private:
    static void postprocess();

    #if ENABLED(EEPROM_SETTINGS)

      static bool eeprom_error, validating;

      static bool _load();
      static bool size_error(const uint16_t size);

      static int eeprom_index;
      static uint16_t working_crc;

      static bool EEPROM_START(int eeprom_offset) {
        if (!persistentStore.access_start()) { SERIAL_ECHO_MSG("No EEPROM."); return false; }
        eeprom_index = eeprom_offset;
        working_crc = 0;
        return true;
      }

      static void EEPROM_FINISH(void) { persistentStore.access_finish(); }

      template<typename T>
      static void EEPROM_SKIP(const T &VAR) { eeprom_index += sizeof(VAR); }

      template<typename T>
      static void EEPROM_WRITE(const T &VAR) {
        persistentStore.write_data(eeprom_index, (const uint8_t *) &VAR, sizeof(VAR), &working_crc);
      }

      template<typename T>
      static void EEPROM_READ(T &VAR) {
        persistentStore.read_data(eeprom_index, (uint8_t *) &VAR, sizeof(VAR), &working_crc, !validating);
      }

      static void EEPROM_READ(uint8_t *VAR, size_t sizeof_VAR) {
        persistentStore.read_data(eeprom_index, VAR, sizeof_VAR, &working_crc, !validating);
      }

      template<typename T>
      static void EEPROM_READ_ALWAYS(T &VAR) {
        persistentStore.read_data(eeprom_index, (uint8_t *) &VAR, sizeof(VAR), &working_crc);
      }

    #endif // EEPROM_SETTINGS
};

extern mvCNCSettings settings;
