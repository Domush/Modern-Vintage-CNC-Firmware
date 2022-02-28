/**
 * Modern Vintage CNC Firmware
*/
#pragma once

/**
 * fan_control.h - temperature controller
 */

// #include "thermistor/thermistors.h"

#include "../inc/mvCNCConfig.h"

#if ENABLED(AUTO_POWER_CONTROL)
  #include "../feature/power.h"
#endif

#if HAS_FANCHECK
  #include "../feature/fancheck.h"
#endif

#ifndef SOFT_PWM_SCALE
  #define SOFT_PWM_SCALE 0
#endif

#define TOOL_INDEX TERN(TOOL_CHANGE_SUPPORT, e, 0)
#define TOOL_NAME TERN_(TOOL_CHANGE_SUPPORT, e)

/**
 * States for ADC reading in the ISR
 */
enum ADCSensorState : char {
  StartSampling,
  #if HAS_JOY_ADC_X
    PrepareJoy_X, MeasureJoy_X,
  #endif
  #if HAS_JOY_ADC_Y
    PrepareJoy_Y, MeasureJoy_Y,
  #endif
  #if HAS_JOY_ADC_Z
    PrepareJoy_Z, MeasureJoy_Z,
  #endif
  #if ENABLED(POWER_MONITOR_CURRENT)
    Prepare_POWER_MONITOR_CURRENT,
    Measure_POWER_MONITOR_CURRENT,
  #endif
  #if ENABLED(POWER_MONITOR_VOLTAGE)
    Prepare_POWER_MONITOR_VOLTAGE,
    Measure_POWER_MONITOR_VOLTAGE,
  #endif
  #if HAS_ADC_BUTTONS
    Prepare_ADC_KEY, Measure_ADC_KEY,
  #endif
  SensorsReady, // Temperatures ready. Delay the next round of readings to let ADC pins settle.
  StartupDelay  // Startup, delay initial temp reading a tiny bit so the hardware can settle
};

// Minimum number of FanControl::ISR loops between sensor readings.
// Multiplied by 16 (OVERSAMPLENR) to obtain the total time to
// get all oversampled sensor readings
#define MIN_ADC_ISR_LOOPS 10

#define ACTUAL_ADC_SAMPLES _MAX(int(MIN_ADC_ISR_LOOPS), int(SensorsReady))

#if HAS_AUTO_FAN || HAS_FANCHECK
  #define HAS_FAN_LOGIC 1
#endif

class FanControl {

  public:

    #if ENABLED(FAN_SOFT_PWM)
      static uint8_t soft_pwm_amount_fan[FAN_COUNT],
                     soft_pwm_count_fan[FAN_COUNT];
    #endif

    #if BOTH(FAN_SOFT_PWM, USE_CONTROLLER_FAN)
      static uint8_t soft_pwm_controller_speed;
    #endif


    #if HAS_FAN_LOGIC
      static constexpr millis_t fan_update_interval_ms = TERN(HAS_PWMFANCHECK, 5000, TERN(HAS_FANCHECK, 1000, 2500));
    #endif

  private:


    #if HAS_COOLER
      #if ENABLED(WATCH_COOLER)
        static cooler_watch_t watch_cooler;
      #endif
      static millis_t next_cooler_check_ms, cooler_fan_flush_ms;
      static int16_t mintemp_raw_COOLER, maxtemp_raw_COOLER;
    #endif

    #if HAS_FAN_LOGIC
      static millis_t fan_update_ms;

      static void manage_extruder_fans(millis_t ms) {
        if (ELAPSED(ms, fan_update_ms)) { // only need to check fan state very infrequently
          const millis_t next_ms = ms + fan_update_interval_ms;
          #if HAS_PWMFANCHECK
            #define FAN_CHECK_DURATION 100
            if (fan_check.is_measuring()) {
              fan_check.compute_speed(ms + FAN_CHECK_DURATION - fan_update_ms);
              fan_update_ms = next_ms;
            }
            else
              fan_update_ms = ms + FAN_CHECK_DURATION;
            fan_check.toggle_measuring();
          #else
            TERN_(HAS_FANCHECK, fan_check.compute_speed(next_ms - fan_update_ms));
            fan_update_ms = next_ms;
          #endif
          TERN_(HAS_AUTO_FAN, update_autofans()); // Needed as last when HAS_PWMFANCHECK to properly force fan speed
        }
      }
    #endif

  public:
    /**
     * Instance Methods
     */

    void init();

    /**
     * Static (class) methods
     */

    #if HAS_FAN

      static uint8_t fan_speed[FAN_COUNT];
      #define FANS_LOOP(I) LOOP_L_N(I, FAN_COUNT)

      static void fanSpeedSet(const uint8_t fan, const uint16_t speed);

      #if ENABLED(REPORT_FAN_CHANGE)
        static void fanSpeedReport(const uint8_t fan);
      #endif

      #if EITHER(PROBING_FANS_OFF, ADVANCED_PAUSE_FANS_PAUSE)
        static bool fans_paused;
        static uint8_t saved_fan_speed[FAN_COUNT];
      #endif

      #if ENABLED(ADAPTIVE_FAN_SLOWING)
        static uint8_t fan_speed_scaler[FAN_COUNT];
      #endif

      static uint8_t scaledFanSpeed(const uint8_t fan, const uint8_t fs) {
        UNUSED(fan); // Potentially unused!
        return (fs * uint16_t(TERN(ADAPTIVE_FAN_SLOWING, fan_speed_scaler[fan], 128))) >> 7;
      }

      static uint8_t scaledFanSpeed(const uint8_t fan) {
        return scaledFanSpeed(fan, fan_speed[fan]);
      }

      static constexpr inline uint8_t pwmToPercent(const uint8_t speed) { return ui8_to_percent(speed); }
      static uint8_t fanSpeedPercent(const uint8_t fan)          { return ui8_to_percent(fan_speed[fan]); }
      static uint8_t scaledFanSpeedPercent(const uint8_t fan)    { return ui8_to_percent(scaledFanSpeed(fan)); }

      #if ENABLED(EXTRA_FAN_SPEED)
        typedef struct { uint8_t saved, speed; } extra_fan_t;
        static extra_fan_t extra_fan_speed[FAN_COUNT];
        static void fanSpeedOverride(const uint8_t fan, const uint16_t command_or_speed);
      #endif

      #if EITHER(PROBING_FANS_OFF, ADVANCED_PAUSE_FANS_PAUSE)
        void fanPause(const bool p);
      #endif

    #endif // HAS_FAN

    static void zero_fan_speeds() {
      #if HAS_FAN
        FANS_LOOP(i) fanSpeedSet(i, 0);
      #endif
    }


  private:
};

extern FanControl fanManager;
