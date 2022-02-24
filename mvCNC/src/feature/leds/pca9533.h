/*
 * Modern Vintage CNC Firmware
 * Copyright (c) 2022 Edward Webber [https://github.com/Domush/mvCNC-Modern-Vintage-CNC-Firmware]

 * Based on Marlin and grbl.
 */
#pragma once

/*
 * Driver for the PCA9533 LED controller found on the MightyBoard
 * used by FlashForge Creator Pro, MakerBot, etc.
 * Written 2020 APR 01 by grauerfuchs
 */
#include <Arduino.h>

#define ENABLE_I2C_PULLUPS

// Chip address (for Wire)
#define PCA9533_Addr        0xC4

// Control registers
#define PCA9533_REG_READ    0x00
#define PCA9533_REG_PSC0    0x01
#define PCA9533_REG_PWM0    0x02
#define PCA9533_REG_PSC1    0x03
#define PCA9533_REG_PWM1    0x04
#define PCA9533_REG_SEL     0x05
#define PCA9533_REGM_AI     0x10

// LED selector operation
#define PCA9533_LED_OP_OFF  0B00
#define PCA9533_LED_OP_ON   0B01
#define PCA9533_LED_OP_PWM0 0B10
#define PCA9533_LED_OP_PWM1 0B11

// Select register bit offsets for LED colors
#define PCA9533_LED_OFS_RED 0
#define PCA9533_LED_OFS_GRN 2
#define PCA9533_LED_OFS_BLU 4

void PCA9533_init();
void PCA9533_reset();
void PCA9533_set_rgb(uint8_t red, uint8_t green, uint8_t blue);
void PCA9533_setOff();
