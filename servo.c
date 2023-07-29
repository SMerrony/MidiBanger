/**
 * SPDX-FileCopyrightText: 2023 Stephen Merrony
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "servo.h"

#include "hardware/clocks.h"
#include "hardware/pwm.h"

static servo_t *servo_map[127] = {0};  // maps MIDI notes to servos

/* setup_servo initialises a servo_t struct according to the passed parameters and then start the PWM
               by calling servo_start.  It must be called before using the servo. 
               The duty parameters are expressed in microseconds, the angles in degrees. */
void servo_setup(servo_t *servo, uint pin, uint16_t note, uint16_t zero_duty, uint16_t full_duty, uint16_t min_angle, uint16_t max_angle) {
    servo->pin = pin;
    servo->zero_duty = zero_duty;
    servo->full_duty = full_duty;
    servo->min_angle = min_angle;
    servo->max_angle = max_angle;

    // precalculate the ranges for efficiency when setting the angle
    servo->duty_range = full_duty - zero_duty;
    servo->angle_range = max_angle - min_angle;

    gpio_set_function(servo->pin, GPIO_FUNC_PWM);
    servo->slice_num = pwm_gpio_to_slice_num(servo->pin);

    uint32_t clock  = clock_get_hz(clk_sys);
    uint32_t divisor = clock / (20000 * 50); // for 50Hz

    pwm_config config = pwm_get_default_config();
    pwm_config_set_clkdiv(&config, (float)divisor);
    pwm_config_set_wrap(&config, 20000);
    pwm_init(servo->slice_num, &config, false);

    servo_map[note] = servo;

    servo_start(servo, min_angle);
}

void servo_start(const servo_t *s, uint16_t start_angle) {
    if (s != NULL) {
        const uint chan = pwm_gpio_to_channel(s->pin);
        uint16_t pulse = (start_angle - s->min_angle) * s->full_duty / s->angle_range + s->zero_duty;
        pwm_set_chan_level(s->slice_num, chan, pulse);
        pwm_set_enabled(s->slice_num, true);
    }
}

void servo_stop(const servo_t *servo) {
    if (servo != NULL) {
        pwm_set_enabled(servo->slice_num, false);
    }
}

void servo_set_angle(const servo_t *s, uint16_t angle) {
    if (s != NULL) {
        const uint chan = pwm_gpio_to_channel(s->pin);
        const uint16_t level = (angle - s->min_angle) * s->full_duty / s->angle_range + s->zero_duty;
        pwm_set_chan_level(s->slice_num, chan, level);
    }
}

servo_t * servo_for_note(uint16_t note) {
    return servo_map[note];
}