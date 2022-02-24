/**
 * Modern Vintage CNC Firmware
 * Copyright (c) 2022 Edward Webber [https://github.com/Domush/mvCNC-Modern-Vintage-CNC-Firmware]

 * Based on Marlin and grbl.
 */
#pragma once

#ifdef __cplusplus
  extern "C" {
#endif

void u8g_i2c_init(const uint8_t clock_options);
//uint8_t u8g_i2c_wait(uint8_t mask, uint8_t pos);
uint8_t u8g_i2c_start(uint8_t sla);
uint8_t u8g_i2c_send_byte(uint8_t data);
void u8g_i2c_stop();

#ifdef __cplusplus
  }
#endif
