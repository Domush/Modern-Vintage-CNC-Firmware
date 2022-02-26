/**
 * Modern Vintage CNC Firmware
*/

/**
 * wii_i2c.cpp - Wii Nunchuck input / jogging
 */

#include "../inc/mvCNCConfigPre.h"

#if ENABLED(WII_NUNCHUCK_JOGGING)

  #include "wii_i2c.h"
  #include "../inc/mvCNCConfig.h"  // for pins
  #include "../module/planner.h"
  #if HAS_CUTTER
    #include "spindle_laser.h"
  #endif
  #include "lib/wii_i2c/src/NintendoExtensionCtrl.h"

WiiNunchuck wii;

  #if ENABLED(EXTENSIBLE_UI)
    #include "../lcd/extui/ui_api.h"
  #endif

  #if ENABLED(INVERT_WII_X)
    #define WII_X(N) (255 - (N))
  #else
    #define WII_X(N) (N)
  #endif

  #if ENABLED(INVERT_WII_Y)
    #define WII_Y(N) (255 - (N))
  #else
    #define WII_Y(N) (N)
  #endif

  #if ENABLED(INVERT_WII_Z)
    #define WII_Z(N) (255 - (N))
  #else
    #define WII_Z(N) (N)
  #endif

  #if ENABLED(WII_NUNCHUCK_DEBUG)
void WiiNunchuck::report() {
  if (mvcnc_debug_flags == MVCNC_DEBUG_COMMUNICATION) {  // Show debug reports when "M110 S5" is set
    const millis_t _report_delay = 1000;                 // delay in ms

    static millis_t next_run     = 0;
    if (PENDING(millis(), next_run)) return;
    next_run = millis() + _report_delay;

    SERIAL_ECHOPGM("Wii Nunchuck States:");
    if (wii.state.buttonZ()) {
      SERIAL_ECHOPGM_P(SP_X_STR, "idle");
      SERIAL_ECHOPGM_P(SP_Y_STR, "idle");
      SERIAL_ECHOPGM_P(SP_Z_STR, WII_Z(MAX(wii.state.joyX(), wii.state.joyY())));
    } else {
      SERIAL_ECHOPGM_P(SP_X_STR, WII_X(wii.state.joyX()));
      SERIAL_ECHOPGM_P(SP_Y_STR, WII_Y(wii.state.joyY()));
      SERIAL_ECHOPGM_P(SP_Z_STR, "idle");
    }
    #if HAS_WII_EN
    SERIAL_ECHO_TERNARY(READ(WII_EN_PIN), " EN=", "HIGH (dis", "LOW (en", "abled)");
    #endif
    SERIAL_EOL();
    char buffer[80];

    const char cPrint = wii.state.buttonC() ? 'C' : '-';
    const char zPrint = wii.state.buttonZ() ? 'Z' : '-';

    snprintf(buffer, sizeof(buffer), "Raw Values: Joy:(%3u, %3u) | Accel XYZ:(%4u, %4u, %4u) | Buttons: %c%c",
             wii.state.joyX(), wii.state.joyY(), wii.state.accelX(), wii.state.accelY(), wii.state.accelZ(), cPrint,
             zPrint);
    SERIAL_ECHOLN(buffer);
  }
}
  #endif

void WiiNunchuck::connect() {
  // Do nothing if enable pin (active-low) is not LOW
  #if HAS_WII_EN
  if (READ(WII_EN_PIN)) return;
  #endif

  // Delay reconnection attempts to save CPU time and prevent job stuttering
  const millis_t _reconnection_delay = 15000;  // delay in ms

  static millis_t next_run           = 0;
  if (PENDING(millis(), next_run)) return;
  next_run = millis() + _reconnection_delay;

  wii.state.begin();
  wii.state.connect();
}

void WiiNunchuck::calculate(xyz_float_t &norm_jog) {
  // Do nothing if enable pin (active-low) is not LOW
  #if HAS_WII_EN
  if (READ(WII_EN_PIN)) return;
  #endif

  // Attempt a connection if not currently connected
  if (wii.state.getControllerType() == ExtensionType::NoController) { wii.connect(); }

  auto _normalize_wii = [](float &axis_jog, const int16_t joy_value, const int16_t(&wii_limits)[4],
                           const bool is_z_axis = false) {
    if (WITHIN(joy_value, wii_limits[0], wii_limits[3])) {
      // within limits, check deadzone
      if (joy_value > wii_limits[2])
        axis_jog = (joy_value - wii_limits[2]) / float(wii_limits[3] - wii_limits[2]);
      else if (joy_value < wii_limits[1])
        axis_jog = (joy_value - wii_limits[1]) / float(wii_limits[1] - wii_limits[0]);  // negative value
      // If C button is not pressed, reduce the speed of the movement
      if (axis_jog != 0.0f && !wii.state.buttonC()) { axis_jog = axis_jog / WII_SLOW_DIVISER; }
      // Map normal to jog value via quadratic relationship
      axis_jog = SIGN(axis_jog) * sq(axis_jog);
    }
  };

  static constexpr int16_t wii_x_limits[4] = WII_X_LIMITS;
  static constexpr int16_t wii_y_limits[4] = WII_Y_LIMITS;
  static constexpr int16_t wii_z_limits[4] = WII_Z_LIMITS;
  if (wii.state.update()) {
    if (wii.state.buttonZ()) {  // Move Z axis if Z button pressed
      if (wii.state.joyX() < wii_z_limits[1] || wii.state.joyX() > wii_z_limits[2]) {
        // joyX moves the Z axis at half adjusted speed.
        float _half_joy_speed = wii.state.joyX() + (128 - wii.state.joyX()) / 2;
        _normalize_wii(norm_jog.z, WII_Z(_half_joy_speed), wii_z_limits, true);
      } else {
        // joyY moves the Z axis at normal adjusted speed.
        _normalize_wii(norm_jog.z, WII_Z(wii.state.joyY()), wii_z_limits, true);
      }
    } else {  // Move X/Y axis
      _normalize_wii(norm_jog.x, WII_X(wii.state.joyX()), wii_x_limits);
      _normalize_wii(norm_jog.y, WII_Y(wii.state.joyY()), wii_y_limits);
    }
  }
}

  #if ENABLED(POLL_JOG)

void WiiNunchuck::inject_jog_moves() {
  // Recursion barrier
  static bool injecting_now;  // = false;
  if (injecting_now || printJobOngoing()) return;
    #if HAS_CUTTER
  if (cutter.power) return;
    #endif

    #if ENABLED(NO_MOTION_BEFORE_HOMING)
  if (axis_should_home()) return;
    #endif

  static constexpr int QUEUE_DEPTH  = 5;      // Insert up to this many movements
  static constexpr float target_lag = 0.25f,  // Aim for 1/4 second lag
    seg_time = target_lag / QUEUE_DEPTH;      // 0.05 seconds, short segments inserted every 1/20th of a second
  static constexpr millis_t timer_limit_ms = millis_t(seg_time * 500);  // 25 ms minimum delay between insertions

  // The planner can merge/collapse small moves, so the movement queue is unreliable to control the lag
  static millis_t next_run                 = 0;
  if (PENDING(millis(), next_run)) return;
  next_run = millis() + timer_limit_ms;

  // Only inject a command if the planner has fewer than 5 moves and there are no unparsed commands
  if (planner.movesplanned() >= QUEUE_DEPTH || queue.has_commands_queued()) return;

  // Normalized jog values are 0 for no movement and -1 or +1 for as max feedrate (nonlinear relationship)
  // Jog are initialized to zero and handling input can update values but doesn't have to
  // You could use a two-axis wiinunchuck and a one-axis keypad and they might work together
  xyz_float_t norm_jog{0};

  // Use ADC values and defined limits. The active zone is normalized: -1..0 (dead) 0..1
  wii.calculate(norm_jog);
    #if ENABLED(WII_NUNCHUCK_DEBUG)
  wii.report();
    #endif

  // Other non-wiinunchuck poll-based jogging could be implemented here
  // with "jogging" encapsulated as a more general class.

  TERN_(EXTENSIBLE_UI, ExtUI::_jogging_update(norm_jog));

  // norm_jog values of [-1 .. 1] maps linearly to [-feedrate .. feedrate]
  xyz_float_t move_dist{0};
  float hypot2 = 0;
  LOOP_LINEAR_AXES(i) if (norm_jog[i]) {
    move_dist[i] =
      seg_time * norm_jog[i] * TERN(EXTENSIBLE_UI, manual_feedrate_mm_s, planner.settings.max_feedrate_mm_s)[i];
    hypot2 += sq(move_dist[i]);
  }

  if (!UNEAR_ZERO(hypot2)) {
    current_position += move_dist;
    apply_motion_limits(current_position);
    const float length = sqrt(hypot2);
    injecting_now      = true;
    planner.buffer_line(current_position, length / seg_time, active_extruder, length);
    injecting_now = false;
  }
}

  #endif  // POLL_JOG

#endif  // WII_NUNCHUCK
