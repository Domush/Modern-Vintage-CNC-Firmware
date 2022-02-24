/**
 * Modern Vintage CNC Firmware
 * Copyright (c) 2022 Edward Webber [https://github.com/Domush/mvCNC-Modern-Vintage-CNC-Firmware]

 * Based on Marlin and grbl.
 */
#include "../inc/mvCNCConfig.h"

#if ENABLED(I2C_AMMETER)

#include "ammeter.h"

#ifndef I2C_AMMETER_IMAX
  #define I2C_AMMETER_IMAX     0.500  // Calibration range 500 Milliamps
#endif

INA226 ina;

Ammeter ammeter;

float Ammeter::scale;
float Ammeter::current;

void Ammeter::init() {
  ina.begin();
  ina.configure(INA226_AVERAGES_16, INA226_BUS_CONV_TIME_1100US, INA226_SHUNT_CONV_TIME_1100US, INA226_MODE_SHUNT_BUS_CONT);
  ina.calibrate(I2C_AMMETER_SHUNT_RESISTOR, I2C_AMMETER_IMAX);
}

float Ammeter::read() {
  scale = 1;
  current = ina.readShuntCurrent();
  if (current <= 0.0001f) current = 0;  // Clean up least-significant-bit amplification errors
  if (current < 0.1f) scale = 1000;
  return current * scale;
}

#endif // I2C_AMMETER
