/**
 * Modern Vintage CNC Firmware
 * Copyright (c) 2022 Edward Webber [https://github.com/Domush/mvCNC-Modern-Vintage-CNC-Firmware]

 * Based on Marlin and grbl.
 */
// adapted from  I2C/master/master.c example
//   https://www-users.cs.york.ac.uk/~pcc/MCP/HAPR-Course-web/CMSIS/examples/html/master_8c_source.html

#ifdef __PLAT_NATIVE_SIM__

#include <cstdint>

#ifdef __cplusplus
  extern "C" {
#endif

uint8_t u8g_i2c_start(const uint8_t sla) {
  return 1;
}

void u8g_i2c_init(const uint8_t clock_option) {
}

uint8_t u8g_i2c_send_byte(uint8_t data) {
  return 1;
}

void u8g_i2c_stop() {
}

#ifdef __cplusplus
  }
#endif

#endif // __PLAT_NATIVE_SIM__
