/**
 * Modern Vintage CNC Firmware
 * Copyright (c) 2022 Edward Webber [https://github.com/Domush/mvCNC-Modern-Vintage-CNC-Firmware]

 * Based on Marlin and grbl.
 */
/**
 * fancheck.cpp - fan tachometer check
 */

#include "../inc/mvCNCConfig.h"

#if HAS_FANCHECK

#include "fancheck.h"
#include "../module/temperature.h"

bool FanCheck::tacho_state[TACHO_COUNT];
uint16_t FanCheck::edge_counter[TACHO_COUNT];
uint8_t FanCheck::rps[TACHO_COUNT];
FanCheck::TachoError FanCheck::error = FanCheck::TachoError::NONE;
bool FanCheck::enabled;

void FanCheck::init() {
  #define _TACHINIT(N) TERN(E##N##_FAN_TACHO_PULLUP, SET_INPUT_PULLUP, TERN(E##N##_FAN_TACHO_PULLDOWN, SET_INPUT_PULLDOWN, SET_INPUT))(E##N##_FAN_TACHO_PIN)
  #if HAS_E0_FAN_TACHO
    _TACHINIT(0);
  #endif
  #if HAS_E1_FAN_TACHO
    _TACHINIT(1);
  #endif
  #if HAS_E2_FAN_TACHO
    _TACHINIT(2);
  #endif
  #if HAS_E3_FAN_TACHO
    _TACHINIT(3);
  #endif
  #if HAS_E4_FAN_TACHO
    _TACHINIT(4);
  #endif
  #if HAS_E5_FAN_TACHO
    _TACHINIT(5);
  #endif
  #if HAS_E6_FAN_TACHO
    _TACHINIT(6);
  #endif
  #if HAS_E7_FAN_TACHO
    _TACHINIT(7);
  #endif
}

void FanCheck::update_tachometers() {
  bool status;

  #define _TACHO_CASE(N) case N: status = READ(E##N##_FAN_TACHO_PIN); break;
  LOOP_L_N(f, TACHO_COUNT) {
    switch (f) {
      #if HAS_E0_FAN_TACHO
        _TACHO_CASE(0)
      #endif
      #if HAS_E1_FAN_TACHO
        _TACHO_CASE(1)
      #endif
      #if HAS_E2_FAN_TACHO
        _TACHO_CASE(2)
      #endif
      #if HAS_E3_FAN_TACHO
        _TACHO_CASE(3)
      #endif
      #if HAS_E4_FAN_TACHO
        _TACHO_CASE(4)
      #endif
      #if HAS_E5_FAN_TACHO
        _TACHO_CASE(5)
      #endif
      #if HAS_E6_FAN_TACHO
        _TACHO_CASE(6)
      #endif
      #if HAS_E7_FAN_TACHO
        _TACHO_CASE(7)
      #endif
      default: continue;
    }

    if (status != tacho_state[f]) {
      if (measuring) ++edge_counter[f];
      tacho_state[f] = status;
    }
  }
}

void FanCheck::compute_speed(uint16_t elapsedTime) {
  static uint8_t errors_count[TACHO_COUNT];
  static uint8_t fan_reported_errors_msk = 0;

  uint8_t fan_error_msk = 0;
  LOOP_L_N(f, TACHO_COUNT) {
    switch (f) {
      TERN_(HAS_E0_FAN_TACHO, case 0:)
      TERN_(HAS_E1_FAN_TACHO, case 1:)
      TERN_(HAS_E2_FAN_TACHO, case 2:)
      TERN_(HAS_E3_FAN_TACHO, case 3:)
      TERN_(HAS_E4_FAN_TACHO, case 4:)
      TERN_(HAS_E5_FAN_TACHO, case 5:)
      TERN_(HAS_E6_FAN_TACHO, case 6:)
      TERN_(HAS_E7_FAN_TACHO, case 7:)
        // Compute fan speed
        rps[f] = edge_counter[f] * float(250) / elapsedTime;
        edge_counter[f] = 0;

        // Check fan speed
        constexpr int8_t max_extruder_fan_errors = TERN(HAS_PWMFANCHECK, 10000, 5000) / Temperature::fan_update_interval_ms;

        if (rps[f] >= 20 || TERN0(HAS_AUTO_FAN, thermalManager.autofan_speed[f] == 0))
          errors_count[f] = 0;
        else if (errors_count[f] < max_extruder_fan_errors)
          ++errors_count[f];
        else if (enabled)
          SBI(fan_error_msk, f);
        break;
      }
    }

  // Drop the error when all fans are ok
  if (!fan_error_msk && error == TachoError::REPORTED) error = TachoError::FIXED;

  if (error == TachoError::FIXED && !jobIsOngoing() && !jobIsPaused()) {
    error = TachoError::NONE; // if the issue has been fixed while the cnc is idle, reenable immediately
    ui.reset_alert_level();
  }

  if (fan_error_msk & ~fan_reported_errors_msk) {
    // Handle new faults only
    LOOP_L_N(f, TACHO_COUNT) if (TEST(fan_error_msk, f)) report_speed_error(f);
  }
  fan_reported_errors_msk = fan_error_msk;
}

void FanCheck::report_speed_error(uint8_t fan) {
  if (jobIsOngoing()) {
    if (error == TachoError::NONE) {
      if (thermalManager.degTargetHotend(fan) != 0) {
        kill(GET_TEXT_F(MSG_FAN_SPEED_FAULT));
        error = TachoError::REPORTED;
      }
      else
        error = TachoError::DETECTED;   // Plans error for next processed command
    }
  }
  else if (!jobIsPaused()) {
    thermalManager.setTargetHotend(0, fan); // Always disable heating
    if (error == TachoError::NONE) error = TachoError::REPORTED;
  }

  SERIAL_ERROR_MSG(STR_ERR_FANSPEED, fan);
  LCD_ALERTMESSAGE(MSG_FAN_SPEED_FAULT);
}

void FanCheck::print_fan_states() {
  LOOP_L_N(s, 2) {
    LOOP_L_N(f, TACHO_COUNT) {
      switch (f) {
        TERN_(HAS_E0_FAN_TACHO, case 0:)
        TERN_(HAS_E1_FAN_TACHO, case 1:)
        TERN_(HAS_E2_FAN_TACHO, case 2:)
        TERN_(HAS_E3_FAN_TACHO, case 3:)
        TERN_(HAS_E4_FAN_TACHO, case 4:)
        TERN_(HAS_E5_FAN_TACHO, case 5:)
        TERN_(HAS_E6_FAN_TACHO, case 6:)
        TERN_(HAS_E7_FAN_TACHO, case 7:)
          SERIAL_ECHOPGM("E", f);
          if (s == 0)
            SERIAL_ECHOPGM(":", 60 * rps[f], " RPM ");
          else
            SERIAL_ECHOPGM("@:", TERN(HAS_AUTO_FAN, thermalManager.autofan_speed[f], 255), " ");
          break;
      }
    }
  }
  SERIAL_EOL();
}

#if ENABLED(AUTO_REPORT_FANS)
  AutoReporter<FanCheck::AutoReportFan> FanCheck::auto_reporter;
  void FanCheck::AutoReportFan::report() { print_fan_states(); }
#endif

#endif // HAS_FANCHECK
