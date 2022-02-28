/**
 * Modern Vintage CNC Firmware
*/

#include "../inc/mvCNCConfig.h"

#if ENABLED(USE_CONTROLLER_FAN)

#include "controllerfan.h"
#include "../module/stepper.h"
#include "../module/fan_control.h"

ControllerFan controllerFan;

uint8_t ControllerFan::speed;

#if ENABLED(CONTROLLER_FAN_EDITABLE)
  controllerFan_settings_t ControllerFan::settings; // {0}
 #else
   const controllerFan_settings_t &ControllerFan::settings = controllerFan_defaults;
#endif

void ControllerFan::setup() {
  SET_OUTPUT(CONTROLLER_FAN_PIN);
  init();
}

void ControllerFan::fanSpeedSet(const uint8_t s) {
  speed = s < (CONTROLLERFAN_SPEED_MIN) ? 0 : s; // Fan OFF below minimum
}

void ControllerFan::update() {
  static millis_t lastMotorOn = 0,    // Last time a motor was turned on
                  nextMotorCheck = 0; // Last time the state was checked
  const millis_t ms = millis();
  if (ELAPSED(ms, nextMotorCheck)) {
    nextMotorCheck = ms + 2500UL; // Not a time critical function, so only check every 2.5s

    // If any triggers for the controller fan are true...
    //   - At least one stepper driver is enabled
    //   - The heated bed is enabled
    //   - TEMP_SENSOR_BOARD is reporting >= CONTROLLER_FAN_MIN_BOARD_TEMP
    const ena_mask_t axis_mask = TERN(CONTROLLER_FAN_USE_Z_ONLY, _BV(Z_AXIS), (ena_mask_t)~TERN0(CONTROLLER_FAN_IGNORE_Z, _BV(Z_AXIS)));
    if ( (stepper.axis_enabled.bits & axis_mask)
      || TERN0(HAS_HEATED_BED, fanManager.temp_bed.soft_pwm_amount > 0)
      || TERN0(HAS_CONTROLLER_FAN_MIN_BOARD_TEMP, fanManager.wholeDegBoard() >= CONTROLLER_FAN_MIN_BOARD_TEMP)
    ) lastMotorOn = ms; //... set time to NOW so the fan will turn on

    // Fan Settings. Set fan > 0:
    //  - If AutoMode is on and steppers have been enabled for CONTROLLERFAN_IDLE_TIME seconds.
    //  - If System is on idle and idle fan speed settings is activated.
    fanSpeedSet(
      settings.auto_mode && lastMotorOn && PENDING(ms, lastMotorOn + SEC_TO_MS(settings.duration))
      ? settings.active_speed : settings.idle_speed
    );

    #if ENABLED(FAN_SOFT_PWM)
      fanManager.soft_pwm_controller_speed = speed;
    #else
      if (PWM_PIN(CONTROLLER_FAN_PIN))
        set_pwm_duty(pin_t(CONTROLLER_FAN_PIN), speed);
      else
        WRITE(CONTROLLER_FAN_PIN, speed > 0);
    #endif
  }
}

#endif // USE_CONTROLLER_FAN
