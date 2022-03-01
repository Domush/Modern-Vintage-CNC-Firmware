/**
 * Modern Vintage CNC Firmware
*/

/**
 * temperature.cpp - temperature control
 */

// Useful when debugging thermocouples
//#define IGNORE_THERMOCOUPLE_ERRORS

#include "../mvCNCCore.h"
#include "../HAL/shared/Delay.h"
#include "../lcd/mvcncui.h"

#include "fan_control.h"
#include "endstops.h"
#include "planner.h"
#include "jobcounter.h"

#if EITHER(HAS_COOLER, LASER_COOLANT_FLOW_METER)
  #include "../feature/cooler.h"
  #include "../feature/spindle_laser.h"
#endif

#if ENABLED(USE_CONTROLLER_FAN)
  #include "../feature/controllerfan.h"
#endif

#if ENABLED(EMERGENCY_PARSER)
  #include "motion.h"
#endif

#if ENABLED(DWIN_CREALITY_LCD)
  #include "../lcd/e3v2/creality/dwin.h"
#elif ENABLED(DWIN_CREALITY_LCD_ENHANCED)
  #include "../lcd/e3v2/proui/dwin.h"
#endif

#if ENABLED(EXTENSIBLE_UI)
  #include "../lcd/extui/ui_api.h"
#endif

#if ENABLED(HOST_PROMPT_SUPPORT)
  #include "../feature/host_actions.h"
#endif

#if HAS_TEMP_SENSOR
  #include "../gcode/gcode.h"
#endif

#if ENABLED(SPINDLE_PARK_FEATURE)
  #include "../libs/spindle_park.h"
#endif

#if ENABLED(BABYSTEPPING) && DISABLED(INTEGRATED_BABYSTEPPING)
  #include "../feature/babystep.h"
#endif

#if HAS_POWER_MONITOR
  #include "../feature/power_monitor.h"
#endif

#if ENABLED(EMERGENCY_PARSER)
  #include "../feature/e_parser.h"
#endif

#if ENABLED(PRINTER_EVENT_LEDS)
  #include "../feature/leds/cnc_event_leds.h"
#endif

#if ENABLED(JOYSTICK)
  #include "../feature/joystick.h"
#endif

#if ENABLED(SINGLENOZZLE)
  #include "tool_change.h"
#endif

#if USE_BEEPER
  #include "../libs/buzzer.h"
#endif

#if HAS_SERVOS
  #include "servo.h"
#endif

FanControl fanManager;

/**
 * public:
 */


#if BOTH(FAN_SOFT_PWM, USE_CONTROLLER_FAN)
  uint8_t FanControl::soft_pwm_controller_speed;
#endif

// Init fans according to whether they're native PWM or Software PWM
#ifdef BOARD_OPENDRAIN_MOSFETS
  #define _INIT_SOFT_FAN(P) OUT_WRITE_OD(P, FAN_INVERTING ? LOW : HIGH)
#else
  #define _INIT_SOFT_FAN(P) OUT_WRITE(P, FAN_INVERTING ? LOW : HIGH)
#endif
#if ENABLED(FAN_SOFT_PWM)
  #define _INIT_FAN_PIN(P) _INIT_SOFT_FAN(P)
#else
  #define _INIT_FAN_PIN(P) do{ if (PWM_PIN(P)) SET_PWM(P); else _INIT_SOFT_FAN(P); }while(0)
#endif
#if ENABLED(FAST_PWM_FAN)
  #define SET_FAST_PWM_FREQ(P) set_pwm_frequency(P, FAST_PWM_FAN_FREQUENCY)
#else
  #define SET_FAST_PWM_FREQ(P) NOOP
#endif
#define INIT_FAN_PIN(P) do{ _INIT_FAN_PIN(P); SET_FAST_PWM_FREQ(P); }while(0)

// HAS_FAN does not include CONTROLLER_FAN
#if HAS_FAN

  uint8_t FanControl::fan_speed[FAN_COUNT]; // = { 0 }

  #if ENABLED(EXTRA_FAN_SPEED)

    FanControl::extra_fan_t FanControl::extra_fan_speed[FAN_COUNT];

    /**
     * Handle the M106 P<fan> T<speed> command:
     *  T1       = Restore fan speed saved on the last T2
     *  T2       = Save the fan speed, then set to the last T<3-255> value
     *  T<3-255> = Set the "extra fan speed"
     */
    void FanControl::fanSpeedOverride(const uint8_t fan, const uint16_t command_or_speed) {
      switch (command_or_speed) {
        case 1:
          fanSpeedSet(fan, extra_fan_speed[fan].saved);
          break;
        case 2:
          extra_fan_speed[fan].saved = fan_speed[fan];
          fanSpeedSet(fan, extra_fan_speed[fan].speed);
          break;
        default:
          extra_fan_speed[fan].speed = _MIN(command_or_speed, 255U);
          break;
      }
    }

  #endif

  #if EITHER(PROBING_FANS_OFF, ADVANCED_PAUSE_FANS_PAUSE)
    bool FanControl::fans_paused; // = false;
    uint8_t FanControl::saved_fan_speed[FAN_COUNT]; // = { 0 }
  #endif

  #if ENABLED(ADAPTIVE_FAN_SLOWING)
    uint8_t FanControl::fan_speed_scaler[FAN_COUNT] = ARRAY_N_1(FAN_COUNT, 128);
  #endif

  /**
   * Set the print fan speed for a target ATC tool
   */
  void FanControl::fanSpeedSet(uint8_t fan, uint16_t speed) {

    NOMORE(speed, 255U);

    if (fan >= FAN_COUNT) return;

    fan_speed[fan] = speed;

    TERN_(REPORT_FAN_CHANGE, fanSpeedReport(fan));
  }

  #if ENABLED(REPORT_FAN_CHANGE)
    /**
     * Report print fan speed for a target ATC tool
     */
    void FanControl::fanSpeedReport(const uint8_t fan) {
      if (fan >= FAN_COUNT) return;
      PORT_REDIRECT(SerialMask::All);
      SERIAL_ECHOLNPGM("M106 P", fan, " S", fan_speed[fan]);
    }
  #endif

  #if EITHER(PROBING_FANS_OFF, ADVANCED_PAUSE_FANS_PAUSE)

    void FanControl::fanPause(const bool pause_fans) {
      if (pause_fans != fans_paused) {
        fans_paused = pause_fans;
        if (pause_fans)
          FANS_LOOP(i) { saved_fan_speed[i] = fan_speed[i]; fan_speed[i] = 0; }
        else
          FANS_LOOP(i) fan_speed[i] = saved_fan_speed[i];
      }
    }

  #endif

#endif // HAS_FAN

/**
 * private:
 */

#if HAS_FAN_LOGIC
  constexpr millis_t FanControl::fan_update_interval_ms;
  millis_t FanControl::fan_update_ms = 0;
#endif

#if ENABLED(FAN_SOFT_PWM)
  uint8_t FanControl::soft_pwm_amount_fan[FAN_COUNT],
          FanControl::soft_pwm_count_fan[FAN_COUNT];
#endif

/**
 * public:
 * Class and Instance Methods
 */

/**
 * Initialize the temperature manager
 *
 * The manager is implemented by periodic calls to manage_heater()
 *
 *  - Init (and disable) SPI thermocouples like MAX6675 and MAX31865
 *  - Disable RUMBA JTAG to accommodate a thermocouple extension
 *  - Read-enable thermistors with a read-enable pin
 *  - Init HEATER and COOLER pins for OUTPUT in OFF state
 *  - Init the FAN pins as PWM or OUTPUT
 *  - Init the SPI interface for SPI thermocouples
 *  - Init ADC according to the HAL
 *  - Set thermistor pins to analog inputs according to the HAL
 *  - Start the Temperature ISR timer
 *  - Init the AUTO FAN pins as PWM or OUTPUT
 *  - Wait 250ms for temperatures to settle
 *  - Init temp_range[], used for catching min/maxtemp
 */
void FanControl::init() {

  #if HAS_COOLER
    OUT_WRITE(COOLER_PIN, COOLER_INVERTING);
  #endif

  #if HAS_FAN0
    INIT_FAN_PIN(FAN_PIN);
  #endif
  #if HAS_FAN1
    INIT_FAN_PIN(FAN1_PIN);
  #endif
  #if HAS_FAN2
    INIT_FAN_PIN(FAN2_PIN);
  #endif
  #if HAS_FAN3
    INIT_FAN_PIN(FAN3_PIN);
  #endif
  #if HAS_FAN4
    INIT_FAN_PIN(FAN4_PIN);
  #endif
  #if HAS_FAN5
    INIT_FAN_PIN(FAN5_PIN);
  #endif
  #if HAS_FAN6
    INIT_FAN_PIN(FAN6_PIN);
  #endif
  #if HAS_FAN7
    INIT_FAN_PIN(FAN7_PIN);
  #endif
  #if ENABLED(USE_CONTROLLER_FAN)
    INIT_FAN_PIN(CONTROLLER_FAN_PIN);
  #endif

  HAL_adc_init();

  #if HAS_JOY_ADC_X
    HAL_ANALOG_SELECT(JOY_X_PIN);
  #endif
  #if HAS_JOY_ADC_Y
    HAL_ANALOG_SELECT(JOY_Y_PIN);
  #endif
  #if HAS_JOY_ADC_Z
    HAL_ANALOG_SELECT(JOY_Z_PIN);
  #endif
  #if HAS_JOY_ADC_EN
    SET_INPUT_PULLUP(JOY_EN_PIN);
  #endif
  #if HAS_ADC_BUTTONS
    HAL_ANALOG_SELECT(ADC_KEYPAD_PIN);
  #endif
  #if ENABLED(POWER_MONITOR_CURRENT)
    HAL_ANALOG_SELECT(POWER_MONITOR_CURRENT_PIN);
  #endif
  #if ENABLED(POWER_MONITOR_VOLTAGE)
    HAL_ANALOG_SELECT(POWER_MONITOR_VOLTAGE_PIN);
  #endif

  #if HAS_COOLER
    while (analog_to_celsius_cooler(mintemp_raw_COOLER) > COOLER_MINTEMP) mintemp_raw_COOLER += TEMPDIR(COOLER) * (OVERSAMPLENR);
    while (analog_to_celsius_cooler(maxtemp_raw_COOLER) < COOLER_MAXTEMP) maxtemp_raw_COOLER -= TEMPDIR(COOLER) * (OVERSAMPLENR);
  #endif

}

/**
 * Timer 0 is shared with millies so don't change the prescaler.
 *
 * On AVR this ISR uses the compare method so it runs at the base
 * frequency (16 MHz / 64 / 256 = 976.5625 Hz), but at the TCNT0 set
 * in OCR0B above (128 or halfway between OVFs).
 *
 *  - Manage PWM to all the heaters and fan
 *  - Prepare or Measure one of the raw ADC sensor values
 *  - Check new temperature values for MIN/MAX errors (kill on error)
 *  - Step the babysteps value for each axis towards 0
 *  - For PINS_DEBUGGING, monitor and report endstop pins
 *  - For ENDSTOP_INTERRUPTS_FEATURE check endstops if flagged
 *  - Call planner.isr to count down its "ignore" time
 */
HAL_TEMP_TIMER_ISR() {
  HAL_timer_isr_prologue(MF_TIMER_TEMP);

  FanControl::isr();

  HAL_timer_isr_epilogue(MF_TIMER_TEMP);
}

#if ENABLED(SLOW_PWM_HEATERS) && !defined(MIN_STATE_TIME)
  #define MIN_STATE_TIME 16 // MIN_STATE_TIME * 65.5 = time in milliseconds
#endif

class SoftPWM {
public:
  uint8_t count;
  inline bool add(const uint8_t mask, const uint8_t amount) {
    count = (count & mask) + amount; return (count > mask);
  }
  #if ENABLED(SLOW_PWM_HEATERS)
    bool state_heater;
    uint8_t state_timer_heater;
    inline void dec() { if (state_timer_heater > 0) state_timer_heater--; }
    inline bool ready(const bool v) {
      const bool rdy = !state_timer_heater;
      if (rdy && state_heater != v) {
        state_heater = v;
        state_timer_heater = MIN_STATE_TIME;
      }
      return rdy;
    }
  #endif
};

/**
 * Handle various ~1kHz tasks associated with temperature
 *  - Heater PWM (~1kHz with scaler)
 *  - LCD Button polling (~500Hz)
 *  - Start / Read one ADC sensor
 *  - Advance Babysteps
 *  - Endstop polling
 *  - Planner clean buffer
 */
void FanControl::isr() {

  static ADCSensorState adc_sensor_state = StartupDelay;
  static uint8_t pwm_count = _BV(SOFT_PWM_SCALE);

  // avoid multiple loads of pwm_count
  uint8_t pwm_count_tmp = pwm_count;

  #if HAS_ADC_BUTTONS
    static unsigned int raw_ADCKey_value = 0;
    static bool ADCKey_pressed = false;
  #endif

  #if HAS_COOLER
    static SoftPWM soft_pwm_cooler;
  #endif

  #if BOTH(FAN_SOFT_PWM, USE_CONTROLLER_FAN)
    static SoftPWM soft_pwm_controller;
  #endif

  #define WRITE_FAN(n, v) WRITE(FAN##n##_PIN, (v) ^ FAN_INVERTING)

  #if DISABLED(SLOW_PWM_HEATERS)

    #if ANY(HAS_COOLER, FAN_SOFT_PWM)
      constexpr uint8_t pwm_mask = TERN0(SOFT_PWM_DITHER, _BV(SOFT_PWM_SCALE) - 1);
      #define _PWM_MOD(N,S,T) do{                           \
        const bool on = S.add(pwm_mask, T.soft_pwm_amount); \
        WRITE_HEATER_##N(on);                               \
      }while(0)
    #endif

    /**
     * Standard heater PWM modulation
     */
    if (pwm_count_tmp >= 127) {
      pwm_count_tmp -= 127;

      #if HAS_COOLER
        _PWM_MOD(COOLER, soft_pwm_cooler, temp_cooler);
      #endif

      #if BOTH(USE_CONTROLLER_FAN, FAN_SOFT_PWM)
        WRITE(CONTROLLER_FAN_PIN, soft_pwm_controller.add(pwm_mask, soft_pwm_controller_speed));
      #endif

      #if ENABLED(FAN_SOFT_PWM)
        #define _FAN_PWM(N) do{                                     \
          uint8_t &spcf = soft_pwm_count_fan[N];                    \
          spcf = (spcf & pwm_mask) + (soft_pwm_amount_fan[N] >> 1); \
          WRITE_FAN(N, spcf > pwm_mask ? HIGH : LOW);               \
        }while(0)
        #if HAS_FAN0
          _FAN_PWM(0);
        #endif
        #if HAS_FAN1
          _FAN_PWM(1);
        #endif
        #if HAS_FAN2
          _FAN_PWM(2);
        #endif
        #if HAS_FAN3
          _FAN_PWM(3);
        #endif
        #if HAS_FAN4
          _FAN_PWM(4);
        #endif
        #if HAS_FAN5
          _FAN_PWM(5);
        #endif
        #if HAS_FAN6
          _FAN_PWM(6);
        #endif
        #if HAS_FAN7
          _FAN_PWM(7);
        #endif
      #endif
    }
    else {
      #define _PWM_LOW(N,S) do{ if (S.count <= pwm_count_tmp) WRITE_HEATER_##N(LOW); }while(0)

      #if HAS_COOLER
        _PWM_LOW(COOLER, soft_pwm_cooler);
      #endif

      #if ENABLED(FAN_SOFT_PWM)
        #if HAS_FAN0
          if (soft_pwm_count_fan[0] <= pwm_count_tmp) WRITE_FAN(0, LOW);
        #endif
        #if HAS_FAN1
          if (soft_pwm_count_fan[1] <= pwm_count_tmp) WRITE_FAN(1, LOW);
        #endif
        #if HAS_FAN2
          if (soft_pwm_count_fan[2] <= pwm_count_tmp) WRITE_FAN(2, LOW);
        #endif
        #if HAS_FAN3
          if (soft_pwm_count_fan[3] <= pwm_count_tmp) WRITE_FAN(3, LOW);
        #endif
        #if HAS_FAN4
          if (soft_pwm_count_fan[4] <= pwm_count_tmp) WRITE_FAN(4, LOW);
        #endif
        #if HAS_FAN5
          if (soft_pwm_count_fan[5] <= pwm_count_tmp) WRITE_FAN(5, LOW);
        #endif
        #if HAS_FAN6
          if (soft_pwm_count_fan[6] <= pwm_count_tmp) WRITE_FAN(6, LOW);
        #endif
        #if HAS_FAN7
          if (soft_pwm_count_fan[7] <= pwm_count_tmp) WRITE_FAN(7, LOW);
        #endif
        #if ENABLED(USE_CONTROLLER_FAN)
          if (soft_pwm_controller.count <= pwm_count_tmp) WRITE(CONTROLLER_FAN_PIN, LOW);
        #endif
      #endif
    }

    // SOFT_PWM_SCALE to frequency:
    //
    // 0: 16000000/64/256/128 =   7.6294 Hz
    // 1:                / 64 =  15.2588 Hz
    // 2:                / 32 =  30.5176 Hz
    // 3:                / 16 =  61.0352 Hz
    // 4:                /  8 = 122.0703 Hz
    // 5:                /  4 = 244.1406 Hz
    pwm_count = pwm_count_tmp + _BV(SOFT_PWM_SCALE);

  #else // SLOW_PWM_HEATERS

    /**
     * SLOW PWM HEATERS
     *
     * For relay-driven heaters
     */
    #define _SLOW_SET(NR,PWM,V) do{ if (PWM.ready(V)) WRITE_HEATER_##NR(V); }while(0)
    #define _SLOW_PWM(NR,PWM,SRC) do{ PWM.count = SRC.soft_pwm_amount; _SLOW_SET(NR,PWM,(PWM.count > 0)); }while(0)
    #define _PWM_OFF(NR,PWM) do{ if (PWM.count < slow_pwm_count) _SLOW_SET(NR,PWM,0); }while(0)

    static uint8_t slow_pwm_count = 0;

    if (slow_pwm_count == 0) {

      #if HAS_COOLER
        _SLOW_PWM(COOLER, soft_pwm_cooler, temp_cooler);
      #endif

    } // slow_pwm_count == 0

    #if HAS_COOLER
      _PWM_OFF(COOLER, soft_pwm_cooler, temp_cooler);
    #endif

    #if ENABLED(FAN_SOFT_PWM)
      if (pwm_count_tmp >= 127) {
        pwm_count_tmp = 0;
        #define _PWM_FAN(N) do{                                 \
          soft_pwm_count_fan[N] = soft_pwm_amount_fan[N] >> 1;  \
          WRITE_FAN(N, soft_pwm_count_fan[N] > 0 ? HIGH : LOW); \
        }while(0)
        #if HAS_FAN0
          _PWM_FAN(0);
        #endif
        #if HAS_FAN1
          _PWM_FAN(1);
        #endif
        #if HAS_FAN2
          _PWM_FAN(2);
        #endif
        #if HAS_FAN3
          _FAN_PWM(3);
        #endif
        #if HAS_FAN4
          _FAN_PWM(4);
        #endif
        #if HAS_FAN5
          _FAN_PWM(5);
        #endif
        #if HAS_FAN6
          _FAN_PWM(6);
        #endif
        #if HAS_FAN7
          _FAN_PWM(7);
        #endif
      }
      #if HAS_FAN0
        if (soft_pwm_count_fan[0] <= pwm_count_tmp) WRITE_FAN(0, LOW);
      #endif
      #if HAS_FAN1
        if (soft_pwm_count_fan[1] <= pwm_count_tmp) WRITE_FAN(1, LOW);
      #endif
      #if HAS_FAN2
        if (soft_pwm_count_fan[2] <= pwm_count_tmp) WRITE_FAN(2, LOW);
      #endif
      #if HAS_FAN3
        if (soft_pwm_count_fan[3] <= pwm_count_tmp) WRITE_FAN(3, LOW);
      #endif
      #if HAS_FAN4
        if (soft_pwm_count_fan[4] <= pwm_count_tmp) WRITE_FAN(4, LOW);
      #endif
      #if HAS_FAN5
        if (soft_pwm_count_fan[5] <= pwm_count_tmp) WRITE_FAN(5, LOW);
      #endif
      #if HAS_FAN6
        if (soft_pwm_count_fan[6] <= pwm_count_tmp) WRITE_FAN(6, LOW);
      #endif
      #if HAS_FAN7
        if (soft_pwm_count_fan[7] <= pwm_count_tmp) WRITE_FAN(7, LOW);
      #endif
    #endif // FAN_SOFT_PWM

    // SOFT_PWM_SCALE to frequency:
    //
    // 0: 16000000/64/256/128 =   7.6294 Hz
    // 1:                / 64 =  15.2588 Hz
    // 2:                / 32 =  30.5176 Hz
    // 3:                / 16 =  61.0352 Hz
    // 4:                /  8 = 122.0703 Hz
    // 5:                /  4 = 244.1406 Hz
    pwm_count = pwm_count_tmp + _BV(SOFT_PWM_SCALE);

    // increment slow_pwm_count only every 64th pwm_count,
    // i.e. yielding a PWM frequency of 16/128 Hz (8s).
    if (((pwm_count >> SOFT_PWM_SCALE) & 0x3F) == 0) {
      slow_pwm_count++;
      slow_pwm_count &= 0x7F;

      TERN_(HAS_COOLER, soft_pwm_cooler.dec());
    }

  #endif // SLOW_PWM_HEATERS

  //
  // Update lcd buttons 488 times per second
  //
  static bool do_buttons;
  if ((do_buttons ^= true)) ui.update_buttons();

  /**
   * One sensor is sampled on every other call of the ISR.
   * Each sensor is read 16 (OVERSAMPLENR) times, taking the average.
   *
   * On each Prepare pass, ADC is started for a sensor pin.
   * On the next pass, the ADC value is read and accumulated.
   *
   * This gives each ADC 0.9765ms to charge up.
   */
  #define ACCUMULATE_ADC(obj) do{ \
    if (!HAL_ADC_READY()) next_sensor_state = adc_sensor_state; \
    else obj.sample(HAL_READ_ADC()); \
  }while(0)

  ADCSensorState next_sensor_state = adc_sensor_state < SensorsReady ? (ADCSensorState)(int(adc_sensor_state) + 1) : StartSampling;

  switch (adc_sensor_state) {

    case SensorsReady: {
      // All sensors have been read. Stay in this state for a few
      // ISRs to save on calls to temp update/checking code below.
      constexpr int8_t extra_loops = 10 - (int8_t)SensorsReady;
      static uint8_t delay_count = 0;
      if (extra_loops > 0) {
        if (delay_count == 0) delay_count = extra_loops;  // Init this delay
        if (--delay_count)                                // While delaying...
          next_sensor_state = SensorsReady;               // retain this state (else, next state will be 0)
        break;
      }
      else {
        adc_sensor_state = StartSampling;                 // Fall-through to start sampling
        next_sensor_state = (ADCSensorState)(int(StartSampling) + 1);
      }
    }

    #if HAS_TEMP_ADC_COOLER
      case PrepareTemp_COOLER: HAL_START_ADC(TEMP_COOLER_PIN); break;
      case MeasureTemp_COOLER: ACCUMULATE_ADC(temp_cooler); break;
    #endif

    #if ENABLED(POWER_MONITOR_CURRENT)
      case Prepare_POWER_MONITOR_CURRENT:
        HAL_START_ADC(POWER_MONITOR_CURRENT_PIN);
        break;
      case Measure_POWER_MONITOR_CURRENT:
        if (!HAL_ADC_READY()) next_sensor_state = adc_sensor_state; // Redo this state
        else power_monitor.add_current_sample(HAL_READ_ADC());
        break;
    #endif

    #if ENABLED(POWER_MONITOR_VOLTAGE)
      case Prepare_POWER_MONITOR_VOLTAGE:
        HAL_START_ADC(POWER_MONITOR_VOLTAGE_PIN);
        break;
      case Measure_POWER_MONITOR_VOLTAGE:
        if (!HAL_ADC_READY()) next_sensor_state = adc_sensor_state; // Redo this state
        else power_monitor.add_voltage_sample(HAL_READ_ADC());
        break;
    #endif

    #if HAS_JOY_ADC_X
      case PrepareJoy_X: HAL_START_ADC(JOY_X_PIN); break;
      case MeasureJoy_X: ACCUMULATE_ADC(joystick.x); break;
    #endif

    #if HAS_JOY_ADC_Y
      case PrepareJoy_Y: HAL_START_ADC(JOY_Y_PIN); break;
      case MeasureJoy_Y: ACCUMULATE_ADC(joystick.y); break;
    #endif

    #if HAS_JOY_ADC_Z
      case PrepareJoy_Z: HAL_START_ADC(JOY_Z_PIN); break;
      case MeasureJoy_Z: ACCUMULATE_ADC(joystick.z); break;
    #endif

    #if HAS_ADC_BUTTONS
      #ifndef ADC_BUTTON_DEBOUNCE_DELAY
        #define ADC_BUTTON_DEBOUNCE_DELAY 16
      #endif
      case Prepare_ADC_KEY: HAL_START_ADC(ADC_KEYPAD_PIN); break;
      case Measure_ADC_KEY:
        if (!HAL_ADC_READY())
          next_sensor_state = adc_sensor_state; // redo this state
        else if (ADCKey_count < ADC_BUTTON_DEBOUNCE_DELAY) {
          raw_ADCKey_value = HAL_READ_ADC();
          if (raw_ADCKey_value <= 900UL * HAL_ADC_RANGE / 1024UL) {
            NOMORE(current_ADCKey_raw, raw_ADCKey_value);
            ADCKey_count++;
          }
          else { //ADC Key release
            if (ADCKey_count > 0) ADCKey_count++; else ADCKey_pressed = false;
            if (ADCKey_pressed) {
              ADCKey_count = 0;
              current_ADCKey_raw = HAL_ADC_RANGE;
            }
          }
        }
        if (ADCKey_count == ADC_BUTTON_DEBOUNCE_DELAY) ADCKey_pressed = true;
        break;
    #endif // HAS_ADC_BUTTONS

    case StartSampling: break;
    case StartupDelay: break;

  } // switch(adc_sensor_state)

  // Go to the next state
  adc_sensor_state = next_sensor_state;

  //
  // Additional ~1kHz Tasks
  //

  #if ENABLED(BABYSTEPPING) && DISABLED(INTEGRATED_BABYSTEPPING)
    babystep.task();
  #endif

  // Check fan tachometers
  TERN_(HAS_FANCHECK, fan_check.update_tachometers());

  // Poll endstops state, if required
  endstops.poll();

  // Periodically call the planner timer service routine
  planner.isr();
}
