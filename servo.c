/**
 * SPDX-FileCopyrightText: 2023 Stephen Merrony
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "servo.h"

#include "hardware/clocks.h"
#include "hardware/pwm.h"

static uint servo_angle_to_usec(uint angle) {
    uint us;
    us = (angle - SERVO_MIN_ANGLE) * (SERVO_MAX_DUTY - SERVO_MIN_DUTY) / (SERVO_MAX_ANGLE - SERVO_MIN_ANGLE) + SERVO_MIN_DUTY;
    return us;
}

servo_t setup_servo(uint pin) {
    servo_t s = {0, 0};
    s.pin = pin;

    gpio_set_function(s.pin, GPIO_FUNC_PWM);
    s.slice_num = pwm_gpio_to_slice_num(s.pin);

    uint32_t clock  = clock_get_hz(clk_sys);
    uint32_t divisor = clock / (20000 * 50); // for 50Hz

    pwm_config config = pwm_get_default_config();
    pwm_config_set_clkdiv(&config, (float)divisor);
    pwm_config_set_wrap(&config, 20000);
    pwm_init(s.slice_num, &config, false);

    servo_start(&s, 0);

    return s;
}

void servo_start(const servo_t *servo, uint start_angle) {
    const uint chan = pwm_gpio_to_channel(servo->pin);
    uint pulse = servo_angle_to_usec(start_angle);
    pwm_set_chan_level(servo->slice_num, chan, pulse);
    pwm_set_enabled(servo->slice_num, true);
}

void servo_stop(const servo_t *servo) {
    pwm_set_enabled(servo->slice_num, false);
}

void servo_set_angle(const servo_t *servo, uint angle) {
    const uint chan = pwm_gpio_to_channel(servo->pin);
    uint pulse = servo_angle_to_usec(angle);
    pwm_set_chan_level(servo->slice_num, chan, pulse);
}