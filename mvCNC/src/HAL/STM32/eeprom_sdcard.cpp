/**
 * Modern Vintage CNC Firmware
*/

#include "../platforms.h"

#ifdef HAL_STM32

/**
 * Implementation of EEPROM settings in SD Card
 */

#include "../../inc/mvCNCConfig.h"

#if ENABLED(SDCARD_EEPROM_EMULATION)

#include "../shared/eeprom_api.h"
#include "../../sd/cardreader.h"

#define EEPROM_FILENAME "eeprom.dat"

    #ifndef MVCNC_EEPROM_SIZE
      #define MVCNC_EEPROM_SIZE 0x1000  // 4KB
    #endif
size_t PersistentStore::capacity() {
  return MVCNC_EEPROM_SIZE;
}

    #define _ALIGN(x) __attribute__((aligned(x)))
static char _ALIGN(4) HAL_eeprom_data[MVCNC_EEPROM_SIZE];

bool PersistentStore::access_start() {
  if (!card.isMounted()) return false;

  SdFile file, root = card.getroot();
  if (!file.open(&root, EEPROM_FILENAME, O_RDONLY))
    return true;

  int bytes_read = file.read(HAL_eeprom_data, MVCNC_EEPROM_SIZE);
  if (bytes_read < 0) return false;
  for (; bytes_read < MVCNC_EEPROM_SIZE; bytes_read++) HAL_eeprom_data[bytes_read] = 0xFF;
  file.close();
  return true;
}

bool PersistentStore::access_finish() {
  if (!card.isMounted()) return false;

  SdFile file, root = card.getroot();
  int bytes_written = 0;
  if (file.open(&root, EEPROM_FILENAME, O_CREAT | O_WRITE | O_TRUNC)) {
    bytes_written = file.write(HAL_eeprom_data, MVCNC_EEPROM_SIZE);
    file.close();
  }
  return (bytes_written == MVCNC_EEPROM_SIZE);
}

bool PersistentStore::write_data(int &pos, const uint8_t *value, size_t size, uint16_t *crc) {
  for (size_t i = 0; i < size; i++)
    HAL_eeprom_data[pos + i] = value[i];
  crc16(crc, value, size);
  pos += size;
  return false;
}

bool PersistentStore::read_data(int &pos, uint8_t *value, const size_t size, uint16_t *crc, const bool writing/*=true*/) {
  for (size_t i = 0; i < size; i++) {
    uint8_t c = HAL_eeprom_data[pos + i];
    if (writing) value[i] = c;
    crc16(crc, &c, 1);
  }
  pos += size;
  return false;
}

#endif // SDCARD_EEPROM_EMULATION
#endif // HAL_STM32
