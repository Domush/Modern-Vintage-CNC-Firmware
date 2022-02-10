/**
 * Webber Ranch CNC Firmware
 * Copyright (c) 2021 WRCNCFirmware [https://github.com/Domush/Webber-Ranch-CNC-Firmware]
 *
 * Based on Marlin and grbl.
 * Copyright (c) 2011 Camiel Gubbels / Erik van der Zalm
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *
 */

#include "../../inc/WRCNCConfig.h"

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
