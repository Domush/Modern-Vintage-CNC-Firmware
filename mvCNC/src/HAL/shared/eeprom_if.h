/**
 * Modern Vintage CNC Firmware
 *
 * Copyright (c) 2022 Edward Webber [https://github.com/Domush/mvCNC-Modern-Vintage-CNC-Firmware]

 * Based on Marlin and grbl.
 */
#pragma once

//
// EEPROM
//
void eeprom_init();
void eeprom_write_byte(uint8_t *pos, uint8_t value);
uint8_t eeprom_read_byte(uint8_t *pos);
