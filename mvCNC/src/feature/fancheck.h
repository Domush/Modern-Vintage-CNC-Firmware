/**
 * Modern Vintage CNC Firmware
 * Copyright (c) 2022 Edward Webber [https://github.com/Domush/mvCNC-Modern-Vintage-CNC-Firmware]

 * Based on Marlin and grbl.
 */
#pragma once

#include "../inc/mvCNCConfig.h"

#if HAS_FANCHECK

#include "../mvCNCCore.h"
#include "../lcd/mvcncui.h"

#if ENABLED(AUTO_REPORT_FANS)
  #include "../libs/autoreport.h"
#endif

#if ENABLED(PARK_HEAD_ON_PAUSE)
  #include "../gcode/queue.h"
#endif

/**
 * fancheck.h
 */
#define TACHO_COUNT TERN(HAS_E7_FAN_TACHO, 8, TERN(HAS_E6_FAN_TACHO, 7, TERN(HAS_E5_FAN_TACHO, 6, TERN(HAS_E4_FAN_TACHO, 5, TERN(HAS_E3_FAN_TACHO, 4, TERN(HAS_E2_FAN_TACHO, 3, TERN(HAS_E1_FAN_TACHO, 2, 1)))))))

class FanCheck {
  private:

    enum class TachoError : uint8_t { NONE, DETECTED, REPORTED, FIXED };

    #if HAS_PWMFANCHECK
      static bool measuring;  // For future use (3 wires PWM controlled fans)
    #else
      static constexpr bool measuring = true;
    #endif
    static bool tacho_state[TACHO_COUNT];
    static uint16_t edge_counter[TACHO_COUNT];
    static uint8_t rps[TACHO_COUNT];
    static TachoError error;

    static void report_speed_error(uint8_t fan);

  public:

    static bool enabled;

    static void init();
    static void update_tachometers();
    static void compute_speed(uint16_t elapsedTime);
    static void print_fan_states();
    #if HAS_PWMFANCHECK
      static void toggle_measuring() { measuring = !measuring; }
      static bool is_measuring() { return measuring; }
    #endif

    static void check_deferred_error() {
      if (error == TachoError::DETECTED) {
        error = TachoError::REPORTED;
        TERN(PARK_HEAD_ON_PAUSE, queue.inject(F("M125")), kill(GET_TEXT_F(MSG_FAN_SPEED_FAULT)));
      }
    }

    #if ENABLED(AUTO_REPORT_FANS)
      struct AutoReportFan { static void report(); };
      static AutoReporter<AutoReportFan> auto_reporter;
    #endif
};

extern FanCheck fan_check;

#endif // HAS_FANCHECK