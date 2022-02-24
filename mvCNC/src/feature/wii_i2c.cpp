/**
 * Modern Vintage CNC Firmware
*/

/**
 * wiinunchuck.cpp - wiinunchuck input / jogging
 */

#include "../inc/mvCNCConfigPre.h"

#if ENABLED(WII_NUNCHUCK_JOGGING)

#include "wii_i2c.h"

#include "../inc/mvCNCConfig.h"  // for pins
#include "../module/planner.h"

WiiNunchuck wiinunchuck;

#if ENABLED(EXTENSIBLE_UI)
  #include "../lcd/extui/ui_api.h"
#endif

temp_info_t WiiNunchuck::x; // = { 0 }
#if ENABLED(INVERT_WII_X)
  #define WII_X(N) (16383 - (N))
#else
  #define WII_X(N) (N)
#endif
temp_info_t WiiNunchuck::y; // = { 0 }
#if ENABLED(INVERT_WII_Y)
  #define WII_Y(N) (16383 - (N))
#else
  #define WII_Y(N) (N)
#endif
temp_info_t WiiNunchuck::z; // = { 0 }
#if ENABLED(INVERT_WII_Z)
  #define WII_Z(N) (16383 - (N))
#else
  #define WII_Z(N) (N)
#endif

#if ENABLED(WII_NUNCHUCK_DEBUG)
  void WiiNunchuck::report() {
    SERIAL_ECHOPGM("WiiNunchuck");
      SERIAL_ECHOPGM_P(SP_X_STR, WII_X(x.raw));
      SERIAL_ECHOPGM_P(SP_Y_STR, WII_Y(y.raw));
      SERIAL_ECHOPGM_P(SP_Z_STR, WII_Z(z.raw));
    #if HAS_WII_EN
      SERIAL_ECHO_TERNARY(READ(WII_EN_PIN), " EN=", "HIGH (dis", "LOW (en", "abled)");
    #endif
    SERIAL_EOL();
  }
#endif

  void WiiNunchuck::calculate(xyz_float_t &norm_jog) {
    // Do nothing if enable pin (active-low) is not LOW
    #if HAS_WII_EN
      if (READ(WII_EN_PIN)) return;
    #endif

    auto _normalize_wii = [](float &axis_jog, const int16_t raw, const int16_t (&wii_limits)[4]) {
      if (WITHIN(raw, wii_limits[0], wii_limits[3])) {
        // within limits, check deadzone
        if (raw > wii_limits[2])
          axis_jog = (raw - wii_limits[2]) / float(wii_limits[3] - wii_limits[2]);
        else if (raw < wii_limits[1])
          axis_jog = (raw - wii_limits[1]) / float(wii_limits[1] - wii_limits[0]);  // negative value
        // Map normal to jog value via quadratic relationship
        axis_jog = SIGN(axis_jog) * sq(axis_jog);
      }
    };

    static constexpr int16_t wii_x_limits[4] = WII_X_LIMITS;
    _normalize_wii(norm_jog.x, WII_X(x.raw), wii_x_limits);
    static constexpr int16_t wii_y_limits[4] = WII_Y_LIMITS;
    _normalize_wii(norm_jog.y, WII_Y(y.raw), wii_y_limits);
    static constexpr int16_t wii_z_limits[4] = WII_Z_LIMITS;
    _normalize_wii(norm_jog.z, WII_Z(z.raw), wii_z_limits);
  }

#if ENABLED(POLL_JOG)

  void WiiNunchuck::inject_jog_moves() {
    // Recursion barrier
    static bool injecting_now; // = false;
    if (injecting_now) return;

    #if ENABLED(NO_MOTION_BEFORE_HOMING)
      if (axis_should_home())
        return;
    #endif

    static constexpr int QUEUE_DEPTH = 5;                                // Insert up to this many movements
    static constexpr float target_lag = 0.25f,                           // Aim for 1/4 second lag
                           seg_time = target_lag / QUEUE_DEPTH;          // 0.05 seconds, short segments inserted every 1/20th of a second
    static constexpr millis_t timer_limit_ms = millis_t(seg_time * 500); // 25 ms minimum delay between insertions

    // The planner can merge/collapse small moves, so the movement queue is unreliable to control the lag
    static millis_t next_run = 0;
    if (PENDING(millis(), next_run)) return;
    next_run = millis() + timer_limit_ms;

    // Only inject a command if the planner has fewer than 5 moves and there are no unparsed commands
    if (planner.movesplanned() >= QUEUE_DEPTH || queue.has_commands_queued())
      return;

    // Normalized jog values are 0 for no movement and -1 or +1 for as max feedrate (nonlinear relationship)
    // Jog are initialized to zero and handling input can update values but doesn't have to
    // You could use a two-axis wiinunchuck and a one-axis keypad and they might work together
    xyz_float_t norm_jog{0};

    // Use ADC values and defined limits. The active zone is normalized: -1..0 (dead) 0..1
    wiinunchuck.calculate(norm_jog);

    // Other non-wiinunchuck poll-based jogging could be implemented here
    // with "jogging" encapsulated as a more general class.

    TERN_(EXTENSIBLE_UI, ExtUI::_wiinunchuck_update(norm_jog));

    // norm_jog values of [-1 .. 1] maps linearly to [-feedrate .. feedrate]
    xyz_float_t move_dist{0};
    float hypot2 = 0;
    LOOP_LINEAR_AXES(i) if (norm_jog[i]) {
      move_dist[i] = seg_time * norm_jog[i] * TERN(EXTENSIBLE_UI, manual_feedrate_mm_s, planner.settings.max_feedrate_mm_s)[i];
      hypot2 += sq(move_dist[i]);
    }

    if (!UNEAR_ZERO(hypot2)) {
      current_position += move_dist;
      apply_motion_limits(current_position);
      const float length = sqrt(hypot2);
      injecting_now = true;
      planner.buffer_line(current_position, length / seg_time, active_extruder, length);
      injecting_now = false;
    }
  }

#endif // POLL_JOG

#endif // WII_NUNCHUCK
