/**
 * Modern Vintage CNC Firmware
*/

#include "../gcode.h"
#include "../../module/settings.h"
#include "../../core/serial.h"
#include "../../inc/mvCNCConfig.h"

#if ENABLED(CONFIGURATION_EMBEDDING)
  #include "../../sd/SdBaseFile.h"
  #include "../../mczip.h"
#endif

/**
 * M500: Store settings in EEPROM
 */
void GcodeSuite::M500() {
  (void)settings.save();
}

/**
 * M501: Read settings from EEPROM
 */
void GcodeSuite::M501() {
  (void)settings.load();
}

/**
 * M502: Revert to default settings
 */
void GcodeSuite::M502() {
  (void)settings.reset();
}

#if DISABLED(DISABLE_M503)

  /**
   * M503: print settings currently in memory
   *
   *   S<bool> : Include / exclude header comments in the output. (Default: S1)
   *
   * With CONFIGURATION_EMBEDDING:
   *   C<flag> : Save the full mvCNC configuration to SD Card as "mc.zip"
   */
  void GcodeSuite::M503() {
    (void)settings.report(!parser.boolval('S', true));

    #if ENABLED(CONFIGURATION_EMBEDDING)
      if (parser.seen_test('C')) {
        SdBaseFile file;
        const uint16_t size = sizeof(mc_zip);
        // Need to create the config size on the SD card
        if (file.open("mc.zip", O_WRITE|O_CREAT) && file.write(pgm_read_ptr(mc_zip), size) != -1 && file.close())
          SERIAL_ECHO_MSG("Configuration saved as 'mc.zip'");
      }
    #endif
  }

#endif // !DISABLE_M503

#if ENABLED(EEPROM_SETTINGS)

  #if ENABLED(MVCNC_DEV_MODE)
    #include "../../libs/hex_print.h"
  #endif

  /**
   * M504: Validate EEPROM Contents
   */
  void GcodeSuite::M504() {
  #if ENABLED(MVCNC_DEV_MODE)
    const bool dowrite = parser.seenval('W');
    if (dowrite || parser.seenval('R')) {
      uint8_t val = 0;
      int addr    = parser.value_ushort();
      if (dowrite) {
        val = parser.byteval('V');
        persistentStore.write_data(addr, &val);
        SERIAL_ECHOLNPGM("Wrote address ", addr, " with ", val);
      } else {
        if (parser.seenval('T')) {
          const int endaddr = parser.value_ushort();
          while (addr <= endaddr) {
            persistentStore.read_data(addr, &val);
            SERIAL_ECHOLNPGM("0x", hex_word(addr), ":", hex_byte(val));
            addr++;
            safe_delay(10);
          }
          SERIAL_EOL();
        } else {
          persistentStore.read_data(addr, &val);
          SERIAL_ECHOLNPGM("Read address ", addr, " and got ", val);
        }
      }
      return;
      }
    #endif

    if (settings.validate())
      SERIAL_ECHO_MSG("EEPROM OK");
  }

#endif
