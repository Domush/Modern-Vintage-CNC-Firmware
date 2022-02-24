/**
 * Modern Vintage CNC Firmware
 * Copyright (c) 2022 Edward Webber [https://github.com/Domush/mvCNC-Modern-Vintage-CNC-Firmware]

 * Based on Marlin and grbl.
 */
#include "../inc/mvCNCConfig.h"

#if EITHER(HAS_COOLER, LASER_COOLANT_FLOW_METER)

#include "cooler.h"
Cooler cooler;

#if HAS_COOLER
  uint8_t Cooler::mode = 0;
  uint16_t Cooler::capacity;
  uint16_t Cooler::load;
  bool Cooler::enabled = false;
#endif

#if ENABLED(LASER_COOLANT_FLOW_METER)
  bool Cooler::flowmeter = false;
  millis_t Cooler::flowmeter_next_ms; // = 0
  volatile uint16_t Cooler::flowpulses;
  float Cooler::flowrate;
  #if ENABLED(FLOWMETER_SAFETY)
    bool Cooler::flowsafety_enabled = true;
    bool Cooler::flowfault = false;
  #endif
#endif

#endif // HAS_COOLER || LASER_COOLANT_FLOW_METER
