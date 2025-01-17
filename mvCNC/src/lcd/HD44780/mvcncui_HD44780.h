/**
 * Modern Vintage CNC Firmware
*/
#pragma once

/**
 * Hitachi HD44780 display defines and headers
 */

#include "../../inc/mvCNCConfig.h"

#if ENABLED(LCD_I2C_TYPE_PCF8575)

  // NOTE: These are register-mapped pins on the PCF8575 controller, not Arduino pins.
  #define LCD_I2C_PIN_BL  3
  #define LCD_I2C_PIN_EN  2
  #define LCD_I2C_PIN_RW  1
  #define LCD_I2C_PIN_RS  0
  #define LCD_I2C_PIN_D4  4
  #define LCD_I2C_PIN_D5  5
  #define LCD_I2C_PIN_D6  6
  #define LCD_I2C_PIN_D7  7

  #include <Wire.h>
  #include <LCD.h>
  #include <LiquidCrystal_I2C.h>
  #define LCD_CLASS LiquidCrystal_I2C

#elif ENABLED(LCD_I2C_TYPE_MCP23017)

  // For the LED indicators (which may be mapped to different events in update_indicators())
  #define LCD_HAS_STATUS_INDICATORS
  #define LED_A 0x04 //100
  #define LED_B 0x02 //010
  #define LED_C 0x01 //001

  #include <Wire.h>
  #include <LiquidTWI2.h>
  #define LCD_CLASS LiquidTWI2

#elif ENABLED(LCD_I2C_TYPE_MCP23008)

  #include <Wire.h>
  #include <LiquidTWI2.h>
  #define LCD_CLASS LiquidTWI2

#elif ENABLED(LCD_I2C_TYPE_PCA8574)

  #include <LiquidCrystal_I2C.h>
  #define LCD_CLASS LiquidCrystal_I2C

#elif ENABLED(SR_LCD_2W_NL)

  // 2 wire Non-latching LCD SR from:
  // https://bitbucket.org/fmalpartida/new-liquidcrystal/wiki/schematics#!shiftregister-connection
  #include <LCD.h>
  #include <LiquidCrystal_SR.h>
  #define LCD_CLASS LiquidCrystal_SR

#elif ENABLED(SR_LCD_3W_NL)

  // NewLiquidCrystal didn't work, so this uses
  // https://github.com/mikeshub/SailfishLCD

  #include <SailfishLCD.h>
  #define LCD_CLASS LiquidCrystalSerial

#elif ENABLED(LCM1602)

  #include <Wire.h>
  #include <LCD.h>
  #include <LiquidCrystal_I2C.h>
  #define LCD_CLASS LiquidCrystal_I2C

#elif ENABLED(YHCB2004)

  #include <LiquidCrystal_AIP31068_SPI.h>
  #define LCD_CLASS LiquidCrystal_AIP31068_SPI

#else

  // Standard directly connected LCD implementations
  #include <LiquidCrystal.h>
  #define LCD_CLASS LiquidCrystal

#endif

#include "../fontutils.h"
#include "../lcdprint.h"
