/**
 * Modern Vintage CNC Firmware
*/

#include "../gcode.h"

#include "../../module/fan_control.h"
#include "../../module/planner.h"       // for planner.finish_and_disable
#include "../../module/jobcounter.h"  // for print_job_timer.stop
#include "../../lcd/mvcncui.h"         // for LCD_MESSAGE_F

#include "../../inc/mvCNCConfig.h"

#if ENABLED(PSU_CONTROL)
  #include "../queue.h"
  #include "../../feature/power.h"
#endif

#if HAS_SUICIDE
  #include "../../mvCNCCore.h"
#endif

#if ENABLED(PSU_CONTROL)

  /**
   * M80   : Turn on the Power Supply
   * M80 S : Report the current state and exit
   */
  void GcodeSuite::M80() {

    // S: Report the current power supply state and exit
    if (parser.seen('S')) {
      SERIAL_ECHOF(powerManager.psu_on ? F("PS:1\n") : F("PS:0\n"));
      return;
    }

    powerManager.power_on();

    /**
     * If you have a switch on suicide pin, this is useful
     * if you want to start another print with suicide feature after
     * a print without suicide...
     */
    #if HAS_SUICIDE
      OUT_WRITE(SUICIDE_PIN, !SUICIDE_PIN_STATE);
    #endif

    TERN_(HAS_MVCNCUI_MENU, ui.reset_status());
  }

#endif // PSU_CONTROL

/**
 * M81: Turn off Power, including Power Supply, if there is one.
 *
 *      This code should ALWAYS be available for FULL SHUTDOWN!
 */
void GcodeSuite::M81() {
  planner.finish_and_disable();
  fanManager.cooldown();

  print_job_timer.stop();

  #if BOTH(HAS_FAN, PROBING_FANS_OFF)
    fanManager.fans_paused = false;
    ZERO(fanManager.saved_fan_speed);
  #endif

  safe_delay(1000); // Wait 1 second before switching off

  LCD_MESSAGE_F(MACHINE_NAME " " STR_OFF ".");

  bool delayed_power_off = false;

  #if ENABLED(POWER_OFF_TIMER)
    if (parser.seenval('D')) {
      uint16_t delay = parser.value_ushort();
      if (delay > 1) { // skip already observed 1s delay
        delayed_power_off = true;
        powerManager.setPowerOffTimer(SEC_TO_MS(delay - 1));
      }
    }
  #endif

  #if ENABLED(POWER_OFF_WAIT_FOR_COOLDOWN)
    if (parser.boolval('S')) {
      delayed_power_off = true;
      powerManager.setPowerOffOnCooldown(true);
    }
  #endif

  if (delayed_power_off) {
    SERIAL_ECHOLNPGM(STR_DELAYED_POWEROFF);
    return;
  }

  #if HAS_SUICIDE
    suicide();
  #elif ENABLED(PSU_CONTROL)
    powerManager.power_off_soon();
  #endif
}
