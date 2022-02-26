/**
 * Modern Vintage CNC Firmware
*/

/**
 * stepper_dac.cpp - To set stepper current via DAC
 */

#include "../../inc/mvCNCConfig.h"

#if HAS_MOTOR_CURRENT_DAC

#include "stepper_dac.h"
#include "../../mvCNCCore.h" // for SP_X_LBL...

bool dac_present = false;
constexpr xyze_uint8_t dac_order = DAC_STEPPER_ORDER;
xyze_uint_t dac_channel_pct = DAC_MOTOR_CURRENT_DEFAULT;

StepperDAC stepper_dac;

int StepperDAC::init() {
  #if PIN_EXISTS(DAC_DISABLE)
    OUT_WRITE(DAC_DISABLE_PIN, LOW);  // set pin low to enable DAC
  #endif

  mcp4728.init();

  if (mcp4728.simpleCommand(RESET)) return -1;

  dac_present = true;

  mcp4728.setVref_all(DAC_STEPPER_VREF);
  mcp4728.setGain_all(DAC_STEPPER_GAIN);

  if (mcp4728.getDrvPct(0) < 1 || mcp4728.getDrvPct(1) < 1 || mcp4728.getDrvPct(2) < 1 || mcp4728.getDrvPct(3) < 1) {
    mcp4728.setDrvPct(dac_channel_pct);
    mcp4728.eepromWrite();
  }

  return 0;
}

void StepperDAC::set_current_value(const uint8_t channel, uint16_t val) {
  if (!dac_present) return;

  NOMORE(val, uint16_t(DAC_STEPPER_MAX));

  mcp4728.analogWrite(dac_order[channel], val);
  mcp4728.simpleCommand(UPDATE);
}

void StepperDAC::set_current_percent(const uint8_t channel, float val) {
  set_current_value(channel, _MIN(val, 100.0f) * (DAC_STEPPER_MAX) / 100.0f);
}

static float dac_perc(int8_t n) { return mcp4728.getDrvPct(dac_order[n]); }
static float dac_amps(int8_t n) { return mcp4728.getValue(dac_order[n]) * 0.125 * RECIPROCAL(DAC_STEPPER_SENSE * 1000); }

uint8_t StepperDAC::get_current_percent(const AxisEnum axis) { return mcp4728.getDrvPct(dac_order[axis]); }
void StepperDAC::set_current_percents(xyze_uint8_t &pct) {
  LOOP_LOGICAL_AXES(i) dac_channel_pct[i] = pct[dac_order[i]];
  mcp4728.setDrvPct(dac_channel_pct);
}

void StepperDAC::print_values() {
  if (!dac_present) return;
  SERIAL_ECHO_MSG("Stepper current values in % (Amps):");
  SERIAL_ECHO_START();
  SERIAL_ECHOPGM_P(SP_X_LBL, dac_perc(X_AXIS), PSTR(" ("), dac_amps(X_AXIS), PSTR(")"));
  #if HAS_Y_AXIS
    SERIAL_ECHOPGM_P(SP_Y_LBL, dac_perc(Y_AXIS), PSTR(" ("), dac_amps(Y_AXIS), PSTR(")"));
  #endif
  #if HAS_Z_AXIS
    SERIAL_ECHOPGM_P(SP_Z_LBL, dac_perc(Z_AXIS), PSTR(" ("), dac_amps(Z_AXIS), PSTR(")"));
  #endif
}

void StepperDAC::commit_eeprom() {
  if (!dac_present) return;
  mcp4728.eepromWrite();
}

#endif // HAS_MOTOR_CURRENT_DAC
