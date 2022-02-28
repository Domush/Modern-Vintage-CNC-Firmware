/**
 * Modern Vintage CNC Firmware
*/
#pragma once

#include "../inc/mvCNCConfigPre.h"

typedef struct {
  uint8_t   active_speed,    // 0-255 (fullspeed); Speed with enabled stepper motors
            idle_speed;      // 0-255 (fullspeed); Speed after idle period with all motors are disabled
  uint16_t  duration;        // Duration in seconds for the fan to run after all motors are disabled
  bool      auto_mode;       // Default true
} controllerFan_settings_t;

#ifndef CONTROLLERFAN_SPEED_ACTIVE
  #define CONTROLLERFAN_SPEED_ACTIVE 255
#endif
#ifndef CONTROLLERFAN_SPEED_IDLE
  #define CONTROLLERFAN_SPEED_IDLE     0
#endif
#ifndef CONTROLLERFAN_IDLE_TIME
  #define CONTROLLERFAN_IDLE_TIME     60
#endif

static constexpr controllerFan_settings_t controllerFan_defaults = {
  CONTROLLERFAN_SPEED_ACTIVE,
  CONTROLLERFAN_SPEED_IDLE,
  CONTROLLERFAN_IDLE_TIME,
  true
};

#if ENABLED(USE_CONTROLLER_FAN)

class ControllerFan {
  private:
    static uint8_t speed;
    static void fanSpeedSet(const uint8_t s);

  public:
    #if ENABLED(CONTROLLER_FAN_EDITABLE)
      static controllerFan_settings_t settings;
    #else
      static const controllerFan_settings_t &settings;
    #endif
    static bool state() { return speed > 0; }
    static void init() { reset(); }
    static void reset() { TERN_(CONTROLLER_FAN_EDITABLE, settings = controllerFan_defaults); }
    static void setup();
    static void update();
};

extern ControllerFan controllerFan;

#endif
