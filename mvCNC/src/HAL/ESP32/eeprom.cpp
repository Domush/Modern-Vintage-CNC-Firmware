/**
 * Modern Vintage CNC Firmware
*/
#ifdef ARDUINO_ARCH_ESP32

#include "../../inc/mvCNCConfig.h"

#if ENABLED(EEPROM_SETTINGS)

#include "../shared/eeprom_api.h"
#include <EEPROM.h>

    #ifndef MVCNC_EEPROM_SIZE
      #define MVCNC_EEPROM_SIZE 0x1000  // 4KB
    #endif
size_t PersistentStore::capacity() {
  return MVCNC_EEPROM_SIZE;
}

bool PersistentStore::access_start()  { return EEPROM.begin(MVCNC_EEPROM_SIZE); }
bool PersistentStore::access_finish() { EEPROM.end(); return true; }

bool PersistentStore::write_data(int &pos, const uint8_t *value, size_t size, uint16_t *crc) {
  for (size_t i = 0; i < size; i++) {
    EEPROM.write(pos++, value[i]);
    crc16(crc, &value[i], 1);
  }
  return false;
}

bool PersistentStore::read_data(int &pos, uint8_t *value, size_t size, uint16_t *crc, const bool writing/*=true*/) {
  for (size_t i = 0; i < size; i++) {
    uint8_t c = EEPROM.read(pos++);
    if (writing) value[i] = c;
    crc16(crc, &c, 1);
  }
  return false;
}

#endif // EEPROM_SETTINGS
#endif // ARDUINO_ARCH_ESP32
