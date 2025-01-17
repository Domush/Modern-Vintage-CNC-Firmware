/**
 * Modern Vintage CNC Firmware
*/

/**
 * Based on u8g_com_st7920_hw_spi.c
 *
 * Universal 8bit Graphics Library
 *
 * Copyright (c) 2011, olikraus@gmail.com
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 *  * Redistributions of source code must retain the above copyright notice, this list
 *    of conditions and the following disclaimer.
 *
 *  * Redistributions in binary form must reproduce the above copyright notice, this
 *    list of conditions and the following disclaimer in the documentation and/or other
 *    materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND
 * CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifdef TARGET_LPC1768

#include "../../../inc/mvCNCConfigPre.h"

#if IS_U8GLIB_ST7920

#include <U8glib-HAL.h>
#include <SoftwareSPI.h>
#include "../../shared/Delay.h"
#include "../../shared/HAL_SPI.h"

#ifndef LCD_SPI_SPEED
  #define LCD_SPI_SPEED SPI_EIGHTH_SPEED  // About 1 MHz
#endif

static pin_t SCK_pin_ST7920_HAL, MOSI_pin_ST7920_HAL_HAL;
static uint8_t SPI_speed = 0;

static void u8g_com_LPC1768_st7920_write_byte_sw_spi(uint8_t rs, uint8_t val) {
  static uint8_t rs_last_state = 255;
  if (rs != rs_last_state) {
    // Transfer Data (FA) or Command (F8)
    swSpiTransfer(rs ? 0x0FA : 0x0F8, SPI_speed, SCK_pin_ST7920_HAL, -1, MOSI_pin_ST7920_HAL_HAL);
    rs_last_state = rs;
    DELAY_US(40); // Give the controller time to process the data: 20 is bad, 30 is OK, 40 is safe
  }
  swSpiTransfer(val & 0x0F0, SPI_speed, SCK_pin_ST7920_HAL, -1, MOSI_pin_ST7920_HAL_HAL);
  swSpiTransfer(val << 4, SPI_speed, SCK_pin_ST7920_HAL, -1, MOSI_pin_ST7920_HAL_HAL);
}

uint8_t u8g_com_HAL_LPC1768_ST7920_sw_spi_fn(u8g_t *u8g, uint8_t msg, uint8_t arg_val, void *arg_ptr) {
  switch (msg) {
    case U8G_COM_MSG_INIT:
      SCK_pin_ST7920_HAL = u8g->pin_list[U8G_PI_SCK];
      MOSI_pin_ST7920_HAL_HAL = u8g->pin_list[U8G_PI_MOSI];

      u8g_SetPIOutput(u8g, U8G_PI_CS);
      u8g_SetPIOutput(u8g, U8G_PI_SCK);
      u8g_SetPIOutput(u8g, U8G_PI_MOSI);
      u8g_Delay(5);

      SPI_speed = swSpiInit(LCD_SPI_SPEED, SCK_pin_ST7920_HAL, MOSI_pin_ST7920_HAL_HAL);

      u8g_SetPILevel(u8g, U8G_PI_CS, 0);
      u8g_SetPILevel(u8g, U8G_PI_SCK, 0);
      u8g_SetPILevel(u8g, U8G_PI_MOSI, 0);

      u8g->pin_list[U8G_PI_A0_STATE] = 0;       /* initial RS state: command mode */
      break;

    case U8G_COM_MSG_STOP:
      break;

    case U8G_COM_MSG_RESET:
       if (U8G_PIN_NONE != u8g->pin_list[U8G_PI_RESET]) u8g_SetPILevel(u8g, U8G_PI_RESET, arg_val);
      break;

    case U8G_COM_MSG_ADDRESS:                     /* define cmd (arg_val = 0) or data mode (arg_val = 1) */
      u8g->pin_list[U8G_PI_A0_STATE] = arg_val;
      break;

    case U8G_COM_MSG_CHIP_SELECT:
      if (U8G_PIN_NONE != u8g->pin_list[U8G_PI_CS]) u8g_SetPILevel(u8g, U8G_PI_CS, arg_val);  //note: the st7920 has an active high chip select
      break;

    case U8G_COM_MSG_WRITE_BYTE:
      u8g_com_LPC1768_st7920_write_byte_sw_spi(u8g->pin_list[U8G_PI_A0_STATE], arg_val);
      break;

    case U8G_COM_MSG_WRITE_SEQ: {
        uint8_t *ptr = (uint8_t*) arg_ptr;
        while (arg_val > 0) {
          u8g_com_LPC1768_st7920_write_byte_sw_spi(u8g->pin_list[U8G_PI_A0_STATE], *ptr++);
          arg_val--;
        }
      }
      break;

      case U8G_COM_MSG_WRITE_SEQ_P: {
        uint8_t *ptr = (uint8_t*) arg_ptr;
        while (arg_val > 0) {
          u8g_com_LPC1768_st7920_write_byte_sw_spi(u8g->pin_list[U8G_PI_A0_STATE], *ptr++);
          arg_val--;
        }
      }
      break;
  }
  return 1;
}

#endif // IS_U8GLIB_ST7920
#endif // TARGET_LPC1768
