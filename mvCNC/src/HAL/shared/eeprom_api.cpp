/**
 * Modern Vintage CNC Firmware
 *
 * Copyright (c) 2022 Edward Webber [https://github.com/Domush/mvCNC-Modern-Vintage-CNC-Firmware]

 * Based on Marlin and grbl.
 */
#include "../../inc/mvCNCConfigPre.h"

#if EITHER(EEPROM_SETTINGS, SD_FIRMWARE_UPDATE)

  #include "eeprom_api.h"
  PersistentStore persistentStore;

#endif
