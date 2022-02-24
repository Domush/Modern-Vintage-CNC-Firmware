/**
 * Modern Vintage CNC Firmware
 * Copyright (c) 2022 Edward Webber [https://github.com/Domush/mvCNC-Modern-Vintage-CNC-Firmware]

 * Based on Marlin and grbl.
 */
#include "../../inc/mvCNCConfig.h"

#if ENABLED(SERVO_DETACH_GCODE)

#include "../gcode.h"
#include "../../module/servo.h"

/**
 * M282: Detach Servo. P<index>
 */
void GcodeSuite::M282() {

  if (!parser.seenval('P')) return;

  const int servo_index = parser.value_int();
  if (WITHIN(servo_index, 0, NUM_SERVOS - 1))
    DETACH_SERVO(servo_index);
  else
    SERIAL_ECHO_MSG("Servo ", servo_index, " out of range");

}

#endif // SERVO_DETACH_GCODE
