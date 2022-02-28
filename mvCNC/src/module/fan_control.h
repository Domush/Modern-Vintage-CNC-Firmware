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

#define HOTEND_INDEX TERN(TOOL_CHANGE_SUPPORT, e, 0)
#define E_NAME TERN_(TOOL_CHANGE_SUPPORT, e)

// Element identifiers. Positive values are hotends. Negative values are other heaters or coolers.
typedef enum : int8_t {
  H_REDUNDANT = HID_REDUNDANT,
  H_COOLER = HID_COOLER,
  H_PROBE = HID_PROBE,
  H_BOARD = HID_BOARD,
  H_CHAMBER = HID_CHAMBER,
  H_BED = HID_BED,
  H_E0 = HID_E0, H_E1, H_E2, H_E3, H_E4, H_E5, H_E6, H_E7,
  H_NONE = -128
} heater_id_t;

// PID storage
typedef struct { float Kp, Ki, Kd;     } PID_t;
typedef struct { float Kp, Ki, Kd, Kc; } PIDC_t;
typedef struct { float Kp, Ki, Kd, Kf; } PIDF_t;
typedef struct { float Kp, Ki, Kd, Kc, Kf; } PIDCF_t;

typedef
  #if BOTH(PID_EXTRUSION_SCALING, PID_FAN_SCALING)
    PIDCF_t
  #elif ENABLED(PID_EXTRUSION_SCALING)
    PIDC_t
  #elif ENABLED(PID_FAN_SCALING)
    PIDF_t
  #else
    PID_t
  #endif
hotend_pid_t;

#if ENABLED(PID_EXTRUSION_SCALING)
  typedef IF<(LPQ_MAX_LEN > 255), uint16_t, uint8_t>::type lpq_ptr_t;
#endif

#define PID_PARAM(F,H) _PID_##F(TERN(PID_PARAMS_PER_HOTEND, H, 0 & H)) // Always use 'H' to suppress warning
#define _PID_Kp(H) TERN(PIDTEMP, FanControl::temp_hotend[H].pid.Kp, NAN)
#define _PID_Ki(H) TERN(PIDTEMP, FanControl::temp_hotend[H].pid.Ki, NAN)
#define _PID_Kd(H) TERN(PIDTEMP, FanControl::temp_hotend[H].pid.Kd, NAN)
#if ENABLED(PIDTEMP)
  #define _PID_Kc(H) TERN(PID_EXTRUSION_SCALING, FanControl::temp_hotend[H].pid.Kc, 1)
  #define _PID_Kf(H) TERN(PID_FAN_SCALING,       FanControl::temp_hotend[H].pid.Kf, 0)
#else
  #define _PID_Kc(H) 1
  #define _PID_Kf(H) 0
#endif

/**
 * States for ADC reading in the ISR
 */
enum ADCSensorState : char {
  StartSampling,
  #if HAS_TEMP_ADC_0
    PrepareTemp_0, MeasureTemp_0,
  #endif
  #if HAS_TEMP_ADC_BED
    PrepareTemp_BED, MeasureTemp_BED,
  #endif
  #if HAS_TEMP_ADC_CHAMBER
    PrepareTemp_CHAMBER, MeasureTemp_CHAMBER,
  #endif
  #if HAS_TEMP_ADC_COOLER
    PrepareTemp_COOLER, MeasureTemp_COOLER,
  #endif
  #if HAS_TEMP_ADC_PROBE
    PrepareTemp_PROBE, MeasureTemp_PROBE,
  #endif
  #if HAS_TEMP_ADC_BOARD
    PrepareTemp_BOARD, MeasureTemp_BOARD,
  #endif
  #if HAS_TEMP_ADC_REDUNDANT
    PrepareTemp_REDUNDANT, MeasureTemp_REDUNDANT,
  #endif
  #if HAS_TEMP_ADC_1
    PrepareTemp_1, MeasureTemp_1,
  #endif
  #if HAS_TEMP_ADC_2
    PrepareTemp_2, MeasureTemp_2,
  #endif
  #if HAS_TEMP_ADC_3
    PrepareTemp_3, MeasureTemp_3,
  #endif
  #if HAS_TEMP_ADC_4
    PrepareTemp_4, MeasureTemp_4,
  #endif
  #if HAS_TEMP_ADC_5
    PrepareTemp_5, MeasureTemp_5,
  #endif
  #if HAS_TEMP_ADC_6
    PrepareTemp_6, MeasureTemp_6,
  #endif
  #if HAS_TEMP_ADC_7
    PrepareTemp_7, MeasureTemp_7,
  #endif
  #if HAS_JOY_ADC_X
    PrepareJoy_X, MeasureJoy_X,
  #endif
  #if HAS_JOY_ADC_Y
    PrepareJoy_Y, MeasureJoy_Y,
  #endif
  #if HAS_JOY_ADC_Z
    PrepareJoy_Z, MeasureJoy_Z,
  #endif
  #if ENABLED(FILAMENT_WIDTH_SENSOR)
    Prepare_FILWIDTH, Measure_FILWIDTH,
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

#if HAS_PID_HEATING
  #define PID_K2 (1-float(PID_K1))
  #define PID_dT ((OVERSAMPLENR * float(ACTUAL_ADC_SAMPLES)) / TEMP_TIMER_FREQUENCY)

  // Apply the scale factors to the PID values
  #define scalePID_i(i)   ( float(i) * PID_dT )
  #define unscalePID_i(i) ( float(i) / PID_dT )
  #define scalePID_d(d)   ( float(d) / PID_dT )
  #define unscalePID_d(d) ( float(d) * PID_dT )
#endif

#if ENABLED(G26_MESH_VALIDATION) && EITHER(HAS_MVCNCUI_MENU, EXTENSIBLE_UI)
  #define G26_CLICK_CAN_CANCEL 1
#endif

// A temperature sensor
typedef struct TempInfo {
  uint16_t acc;
  int16_t raw;
  celsius_float_t celsius;
  inline void reset() { acc = 0; }
  inline void sample(const uint16_t s) { acc += s; }
  inline void update() { raw = acc; }
} temp_info_t;

#if HAS_TEMP_REDUNDANT
  // A redundant temperature sensor
  typedef struct RedundantTempInfo : public TempInfo {
    temp_info_t* target;
  } redundant_info_t;
#endif

// A PWM heater with temperature sensor
typedef struct HeaterInfo : public TempInfo {
  celsius_t target;
  uint8_t soft_pwm_amount;
} heater_info_t;

// A heater with PID stabilization
template<typename T>
struct PIDHeaterInfo : public HeaterInfo {
  T pid;  // Initialized by settings.load()
};

// Temperature sensor read value ranges

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

      static void set_fan_speed(const uint8_t fan, const uint16_t speed);

      #if ENABLED(REPORT_FAN_CHANGE)
        static void report_fan_speed(const uint8_t fan);
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
        static void set_temp_fan_speed(const uint8_t fan, const uint16_t command_or_speed);
      #endif

      #if EITHER(PROBING_FANS_OFF, ADVANCED_PAUSE_FANS_PAUSE)
        void set_fans_paused(const bool p);
      #endif

    #endif // HAS_FAN

    static void zero_fan_speeds() {
      #if HAS_FAN
        FANS_LOOP(i) set_fan_speed(i, 0);
      #endif
    }


  private:
};

extern FanControl thermalManager;
