/**
 * Modern Vintage CNC Firmware
*/

#include "../../../inc/mvCNCConfig.h"

#if HAS_MOTOR_CURRENT_SPI || HAS_MOTOR_CURRENT_PWM || HAS_MOTOR_CURRENT_I2C || HAS_MOTOR_CURRENT_DAC

#include "../../gcode.h"

#if HAS_MOTOR_CURRENT_SPI || HAS_MOTOR_CURRENT_PWM
  #include "../../../module/stepper.h"
#endif

#if HAS_MOTOR_CURRENT_I2C
  #include "../../../feature/digipot/digipot.h"
#endif

#if HAS_MOTOR_CURRENT_DAC
  #include "../../../feature/dac/stepper_dac.h"
#endif

/**
 * M907: Set digital trimpot motor current using axis codes X [Y] [Z] [E]
 *   B<current> - Special case for 4th (E) axis
 *   S<current> - Special case to set first 3 axes
 */
void GcodeSuite::M907() {
  #if HAS_MOTOR_CURRENT_SPI

    if (!parser.seen("BS" LOGICAL_AXES_STRING))
      return M907_report();

    LOOP_LOGICAL_AXES(i) if (parser.seenval(axis_codes[i])) stepper.set_digipot_current(i, parser.value_int());
    if (parser.seenval('B')) stepper.set_digipot_current(4, parser.value_int());
    if (parser.seenval('S')) LOOP_LE_N(i, 4) stepper.set_digipot_current(i, parser.value_int());

  #elif HAS_MOTOR_CURRENT_PWM

    if (!parser.seen(
      #if ANY_PIN(MOTOR_CURRENT_PWM_X, MOTOR_CURRENT_PWM_Y, MOTOR_CURRENT_PWM_XY)
        "XY"
      #endif
      #if PIN_EXISTS(MOTOR_CURRENT_PWM_Z)
        "Z"
      #endif
      #if PIN_EXISTS(MOTOR_CURRENT_PWM_E)
        "E"
      #endif
    )) return M907_report();

    #if ANY_PIN(MOTOR_CURRENT_PWM_X, MOTOR_CURRENT_PWM_Y, MOTOR_CURRENT_PWM_XY)
      if (parser.seenval('X') || parser.seenval('Y')) stepper.set_digipot_current(0, parser.value_int());
    #endif
    #if PIN_EXISTS(MOTOR_CURRENT_PWM_Z)
      if (parser.seenval('Z')) stepper.set_digipot_current(1, parser.value_int());
    #endif
    #if PIN_EXISTS(MOTOR_CURRENT_PWM_E)
      if (parser.seenval('E')) stepper.set_digipot_current(2, parser.value_int());
    #endif

  #endif // HAS_MOTOR_CURRENT_PWM

  #if HAS_MOTOR_CURRENT_I2C
    // this one uses actual amps in floating point
    LOOP_LOGICAL_AXES(i) if (parser.seenval(axis_codes[i])) digipot_i2c.set_current(i, parser.value_float());
      // Additional ATC tools use B,C,D for channels 4,5,6.
      // TODO: Change these parameters because 'E' is used. B<index>?
  #endif

  #if HAS_MOTOR_CURRENT_DAC
    if (parser.seenval('S')) {
      const float dac_percent = parser.value_float();
      LOOP_LE_N(i, 4) stepper_dac.set_current_percent(i, dac_percent);
    }
    LOOP_LOGICAL_AXES(i) if (parser.seenval(axis_codes[i])) stepper_dac.set_current_percent(i, parser.value_float());
  #endif
}

#if HAS_MOTOR_CURRENT_SPI || HAS_MOTOR_CURRENT_PWM

  void GcodeSuite::M907_report(const bool forReplay/*=true*/) {
    report_heading_etc(forReplay, F(STR_STEPPER_MOTOR_CURRENTS));
    #if HAS_MOTOR_CURRENT_PWM
      SERIAL_ECHOLNPGM_P(                                    // PWM-based has 3 values:
          PSTR("  M907 X"), stepper.motor_current_setting[0]  // X and Y
        , SP_Z_STR,         stepper.motor_current_setting[1]  // Z
        , SP_E_STR,         stepper.motor_current_setting[2]  // E
      );
    #elif HAS_MOTOR_CURRENT_SPI
      SERIAL_ECHOPGM("  M907");                               // SPI-based has 5 values:
      LOOP_LOGICAL_AXES(q) {                                  // X Y Z (I J K) E (map to X Y Z (I J K) E0 by default)
        SERIAL_CHAR(' ', axis_codes[q]);
        SERIAL_ECHO(stepper.motor_current_setting[q]);
      }
      SERIAL_CHAR(' ', 'B');                                  // B (maps to E1 by default)
      SERIAL_ECHOLN(stepper.motor_current_setting[4]);
    #endif
  }

#endif // HAS_MOTOR_CURRENT_SPI || HAS_MOTOR_CURRENT_PWM

#if HAS_MOTOR_CURRENT_SPI || HAS_MOTOR_CURRENT_DAC

  /**
   * M908: Control digital trimpot directly (M908 P<pin> S<current>)
   */
  void GcodeSuite::M908() {
    TERN_(HAS_MOTOR_CURRENT_SPI, stepper.set_digipot_value_spi(parser.intval('P'), parser.intval('S')));
    TERN_(HAS_MOTOR_CURRENT_DAC, stepper_dac.set_current_value(parser.byteval('P', -1), parser.ushortval('S', 0)));
  }

  #if HAS_MOTOR_CURRENT_DAC

    void GcodeSuite::M909() { stepper_dac.print_values(); }
    void GcodeSuite::M910() { stepper_dac.commit_eeprom(); }

  #endif // HAS_MOTOR_CURRENT_DAC

#endif // HAS_MOTOR_CURRENT_SPI || HAS_MOTOR_CURRENT_DAC

#endif // HAS_MOTOR_CURRENT_SPI || HAS_MOTOR_CURRENT_PWM || HAS_MOTOR_CURRENT_I2C || HAS_MOTOR_CURRENT_DAC
