/**
 * Modern Vintage CNC Firmware
 * Copyright (c) 2022 Edward Webber [https://github.com/Domush/mvCNC-Modern-Vintage-CNC-Firmware]

 * Based on Marlin and grbl.
 */
#pragma once

// Pin number of unattached pins
#define NOT_ATTACHED                    (-1)
#define INVALID_SERVO                   255

#ifndef MAX_SERVOS
  #define MAX_SERVOS 3
#endif

#define SERVO_DEFAULT_MIN_PW            544
#define SERVO_DEFAULT_MAX_PW            2400
#define SERVO_DEFAULT_MIN_ANGLE         0
#define SERVO_DEFAULT_MAX_ANGLE         180

#define HAL_SERVO_LIB libServo

class libServo {
  public:
    libServo();
    bool attach(const int32_t pin, const int32_t minAngle=SERVO_DEFAULT_MIN_ANGLE, const int32_t maxAngle=SERVO_DEFAULT_MAX_ANGLE);
    bool attached() const { return pin != NOT_ATTACHED; }
    bool detach();
    void move(const int32_t value);
    int32_t read() const;
  private:
    void servoWrite(uint8_t pin, const uint16_t duty_cycle);

    uint8_t servoIndex;               // index into the channel data for this servo
    int32_t pin = NOT_ATTACHED;
    int32_t minAngle;
    int32_t maxAngle;
    int32_t angle;

    bool setupSoftPWM(const int32_t pin);
    void pauseSoftPWM();
    void pwmSetDuty(const uint16_t duty_cycle);
};
