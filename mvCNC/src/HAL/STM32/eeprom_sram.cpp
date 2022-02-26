/**
 * Modern Vintage CNC Firmware
 *
 * Copyright (c) 2022 Edward Webber [https://github.com/Domush/mvCNC-Modern-Vintage-CNC-Firmware]

 * Based on Marlin and grbl.
 */
#include "../platforms.h"

#ifdef HAL_STM32

#include "../../inc/mvCNCConfig.h"

#if ENABLED(SRAM_EEPROM_EMULATION)

#include "../shared/eeprom_if.h"
#include "../shared/eeprom_api.h"

    #ifndef MVCNC_EEPROM_SIZE
      #define MVCNC_EEPROM_SIZE 0x1000  // 4KB
    #endif
size_t PersistentStore::capacity() {
  return MVCNC_EEPROM_SIZE;
}

bool PersistentStore::access_start()  { return true; }
bool PersistentStore::access_finish() { return true; }

bool PersistentStore::write_data(int &pos, const uint8_t *value, size_t size, uint16_t *crc) {
  while (size--) {
    uint8_t v = *value;

    // Save to Backup SRAM
    *(__IO uint8_t *)(BKPSRAM_BASE + (uint8_t * const)pos) = v;

    crc16(crc, &v, 1);
    pos++;
    value++;
  };

  return false;
}

bool PersistentStore::read_data(int &pos, uint8_t *value, size_t size, uint16_t *crc, const bool writing/*=true*/) {
  do {
    // Read from either external EEPROM, program flash or Backup SRAM
    const uint8_t c = ( *(__IO uint8_t *)(BKPSRAM_BASE + ((uint8_t*)pos)) );
    if (writing) *value = c;
    crc16(crc, &c, 1);
    pos++;
    value++;
  } while (--size);
  return false;
}

#endif // SRAM_EEPROM_EMULATION
#endif // HAL_STM32
