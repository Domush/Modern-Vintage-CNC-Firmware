/**
 * Modern Vintage CNC Firmware
*/

#include "../../inc/mvCNCConfig.h"

#if ALL(HAS_SPI_FLASH, SDSUPPORT, MVCNC_DEV_MODE)

  #include "../gcode.h"
  #include "../../sd/cardreader.h"
  #include "../../libs/W25Qxx.h"

/**
 * M993: Backup SPI Flash to SD
 */
void GcodeSuite::M993() {
  if (!card.isMounted()) card.mount();

  char fname[] = "spiflash.bin";
  card.openFileWrite(fname);
  if (!card.isFileOpen()) {
    SERIAL_ECHOLNPGM("Failed to open ", fname, " to write.");
    return;
  }

  uint8_t buf[1024];
  uint32_t addr = 0;
  W25QXX.init(SPI_QUARTER_SPEED);
  SERIAL_ECHOPGM("Save SPI Flash");
  while (addr < SPI_FLASH_SIZE) {
    W25QXX.SPI_FLASH_BufferRead(buf, addr, COUNT(buf));
    addr += COUNT(buf);
    card.write(buf, COUNT(buf));
    if (addr % (COUNT(buf) * 10) == 0) SERIAL_CHAR('.');
  }
  SERIAL_ECHOLNPGM(" done");

  card.closefile();
}

/**
 * M994: Load a backup from SD to SPI Flash
 */
void GcodeSuite::M994() {
  if (!card.isMounted()) card.mount();

  char fname[] = "spiflash.bin";
  card.openFileRead(fname);
  if (!card.isFileOpen()) {
    SERIAL_ECHOLNPGM("Failed to open ", fname, " to read.");
    return;
  }

  uint8_t buf[1024];
  uint32_t addr = 0;
  W25QXX.init(SPI_QUARTER_SPEED);
  W25QXX.SPI_FLASH_BulkErase();
  SERIAL_ECHOPGM("Load SPI Flash");
  while (addr < SPI_FLASH_SIZE) {
    card.read(buf, COUNT(buf));
    W25QXX.SPI_FLASH_BufferWrite(buf, addr, COUNT(buf));
    addr += COUNT(buf);
    if (addr % (COUNT(buf) * 10) == 0) SERIAL_CHAR('.');
  }
  SERIAL_ECHOLNPGM(" done");

  card.closefile();
}

#endif  // HAS_SPI_FLASH && SDSUPPORT && MVCNC_DEV_MODE
