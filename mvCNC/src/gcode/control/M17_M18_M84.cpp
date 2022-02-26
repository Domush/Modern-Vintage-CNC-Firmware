/**
 * Modern Vintage CNC Firmware
*/

#include "../gcode.h"
#include "../../mvCNCCore.h" // for stepper_inactive_time, disable_e_steppers
#include "../../lcd/mvcncui.h"
#include "../../module/stepper.h"

#if ENABLED(AUTO_BED_LEVELING_UBL)
  #include "../../feature/bedlevel/bedlevel.h"
#endif

#define DEBUG_OUT ENABLED(MVCNC_DEV_MODE)
#include "../../core/debug_out.h"
#include "../../libs/hex_print.h"

inline axis_flags_t selected_axis_bits() {
  axis_flags_t selected{0};
  selected.bits |= LINEAR_AXIS_GANG(
      (parser.seen_test('X')        << X_AXIS),
    | (parser.seen_test('Y')        << Y_AXIS),
    | (parser.seen_test('Z')        << Z_AXIS),
    | (parser.seen_test(AXIS4_NAME) << I_AXIS),
    | (parser.seen_test(AXIS5_NAME) << J_AXIS),
    | (parser.seen_test(AXIS6_NAME) << K_AXIS)
  );
  return selected;
}

// Enable specified axes and warn about other affected axes
void do_enable(const axis_flags_t to_enable) {
  const ena_mask_t was_enabled = stepper.axis_enabled.bits,
                  shall_enable = to_enable.bits & ~was_enabled;

  DEBUG_ECHOLNPGM("Now Enabled: ", hex_word(stepper.axis_enabled.bits), "  Enabling: ", hex_word(to_enable.bits), " | ", shall_enable);

  if (!shall_enable) return;    // All specified axes already enabled?

  ena_mask_t also_enabled = 0;    // Track steppers enabled due to overlap

  // Enable all flagged axes
  LOOP_LINEAR_AXES(a) {
    if (TEST(shall_enable, a)) {
      stepper.enable_axis(AxisEnum(a));         // Mark and enable the requested axis
      DEBUG_ECHOLNPGM("Enabled ", axis_codes[a], " (", a, ") with overlap ", hex_word(enable_overlap[a]), " ... Enabled: ", hex_word(stepper.axis_enabled.bits));
      also_enabled |= enable_overlap[a];
    }
  }
  if ((also_enabled &= ~(shall_enable | was_enabled))) {
    SERIAL_CHAR('(');
    LOOP_LINEAR_AXES(a) if (TEST(also_enabled, a)) SERIAL_CHAR(axis_codes[a], ' ');
    SERIAL_ECHOLNPGM("also enabled)");
  }

  DEBUG_ECHOLNPGM("Enabled Now: ", hex_word(stepper.axis_enabled.bits));
}

/**
 * M17: Enable stepper motor power for one or more axes.
 *      Print warnings for axes that share an ENABLE_PIN.
 *
 * Examples:
 *
 *  M17 XZ ; Enable X and Z axes
 *  M17 E  ; Enable all E steppers
 *  M17 E1 ; Enable just the E1 stepper
 */
void GcodeSuite::M17() {
  if (parser.seen_axis()) {
    if (any_enable_overlap())
      do_enable(selected_axis_bits());
    else {
      LINEAR_AXIS_CODE(
        if (parser.seen_test('X'))        stepper.enable_axis(X_AXIS),
        if (parser.seen_test('Y'))        stepper.enable_axis(Y_AXIS),
        if (parser.seen_test('Z'))        stepper.enable_axis(Z_AXIS),
        if (parser.seen_test(AXIS4_NAME)) stepper.enable_axis(I_AXIS),
        if (parser.seen_test(AXIS5_NAME)) stepper.enable_axis(J_AXIS),
        if (parser.seen_test(AXIS6_NAME)) stepper.enable_axis(K_AXIS)
      );
    }
  }
  else {
    LCD_MESSAGE(MSG_NO_MOVE);
    stepper.enable_all_steppers();
  }
}

void try_to_disable(const axis_flags_t to_disable) {
  ena_mask_t still_enabled = to_disable.bits & stepper.axis_enabled.bits;

  DEBUG_ECHOLNPGM("Enabled: ", hex_word(stepper.axis_enabled.bits), " To Disable: ", hex_word(to_disable.bits), " | ", hex_word(still_enabled));

  if (!still_enabled) return;

  // Attempt to disable all flagged axes
  LOOP_LINEAR_AXES(a)
    if (TEST(to_disable.bits, a)) {
      DEBUG_ECHOPGM("Try to disable ", axis_codes[a], " (", a, ") with overlap ", hex_word(enable_overlap[a]), " ... ");
      if (stepper.disable_axis(AxisEnum(a))) {            // Mark the requested axis and request to disable
        DEBUG_ECHOPGM("OK");
        still_enabled &= ~(_BV(a) | enable_overlap[a]);   // If actually disabled, clear one or more tracked bits
      }
      else
        DEBUG_ECHOPGM("OVERLAP");
      DEBUG_ECHOLNPGM(" ... still_enabled=", hex_word(still_enabled));
    }
  auto overlap_warning = [](const ena_mask_t axis_bits) {
    SERIAL_ECHOPGM(" not disabled. Shared with");
    LOOP_LINEAR_AXES(a) if (TEST(axis_bits, a)) SERIAL_CHAR(' ', axis_codes[a]);
    SERIAL_ECHOLNPGM(".");
  };

  // If any of the requested axes are still enabled, give a warning
  LOOP_LINEAR_AXES(a) {
    if (TEST(still_enabled, a)) {
      SERIAL_CHAR(axis_codes[a]);
      overlap_warning(stepper.axis_enabled.bits & enable_overlap[a]);
    }
  }
  DEBUG_ECHOLNPGM("Enabled Now: ", hex_word(stepper.axis_enabled.bits));
}

/**
 * M18, M84: Disable stepper motor power for one or more axes.
 *           Print warnings for axes that share an ENABLE_PIN.
 */
void GcodeSuite::M18_M84() {
  if (parser.seenval('S')) {
    reset_stepper_timeout();
    stepper_inactive_time = parser.value_millis_from_seconds();
  }
  else {
    if (parser.seen_axis()) {
      planner.synchronize();
      if (any_enable_overlap())
        try_to_disable(selected_axis_bits());
      else {
        LINEAR_AXIS_CODE(
          if (parser.seen_test('X'))        stepper.disable_axis(X_AXIS),
          if (parser.seen_test('Y'))        stepper.disable_axis(Y_AXIS),
          if (parser.seen_test('Z'))        stepper.disable_axis(Z_AXIS),
          if (parser.seen_test(AXIS4_NAME)) stepper.disable_axis(I_AXIS),
          if (parser.seen_test(AXIS5_NAME)) stepper.disable_axis(J_AXIS),
          if (parser.seen_test(AXIS6_NAME)) stepper.disable_axis(K_AXIS)
        );
      }
    }
    else
      planner.finish_and_disable();

    TERN_(AUTO_BED_LEVELING_UBL, ubl.steppers_were_disabled());
  }
}
