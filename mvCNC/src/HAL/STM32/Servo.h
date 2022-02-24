/**
 * Modern Vintage CNC Firmware
 * Copyright (c) 2022 Edward Webber [https://github.com/Domush/mvCNC-Modern-Vintage-CNC-Firmware]

 * Based on Marlin and grbl.
 */
#pragma once

#include <Servo.h>

#include "../../core/millis_t.h"

// Inherit and expand on the official library
class libServo {
  public:
    libServo();
    int8_t attach(const int pin = 0); // pin == 0 uses value from previous call
    int8_t attach(const int pin, const int min, const int max);
    void detach() { stm32_servo.detach(); }
    int read() { return stm32_servo.read(); }
    void move(const int value);

    void pause();
    void resume();

    static void pause_all_servos();
    static void resume_all_servos();
    static void setInterruptPriority(uint32_t preemptPriority, uint32_t subPriority);

  private:
    Servo stm32_servo;

    int servo_pin = 0;
    millis_t delay = 0;

    bool was_attached_before_pause;
    int value_before_pause;
};
