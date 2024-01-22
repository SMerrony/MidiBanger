/**
 * SPDX-FileCopyrightText: 2023 Stephen Merrony
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "servo.h"

#include "hardware/clocks.h"
#include "hardware/pwm.h"

#include "config.h"

static servo_t servos[MAX_SERVOS] = {0};

static int servos_in_use = 0;

/* resetter_task performs the percussive returns.
   It loops over each servo and checks if its reset time has passed.
   N.B. Must not be called until all servo_setup() calls are completed. */
void resetter_task() {
	while (true){
		for (int s = 0; s < servos_in_use; ++s) {
			if (! is_at_the_end_of_time(servos[s].reset)) {
				if (absolute_time_diff_us(servos[s].reset, get_absolute_time()) > 0) {
					servo_set_angle(s, servos[s].min_angle);
					servos[s].reset = at_the_end_of_time;
				}
			}
		}
		busy_wait_ms(2);
	}
}

/* setup_servo initialises a servo_t struct according to the passed parameters and then start the PWM
               by calling servo_start.  It must be called before using the servo. 
               The duty parameters are expressed in microseconds, the angles in degrees.
    @param servo index
    @param pin   GPIO pin number
    @param zero_duty 
    @param full_duty
    @param min_angle in degrees for this 'mallet'
    @param max_angle in degrees */
void servo_setup(int servo, uint pin, uint16_t zero_duty, uint16_t full_duty, uint16_t min_angle, uint16_t max_angle) {
    servos[servo].pin = pin;
    servos[servo].zero_duty = zero_duty;
    servos[servo].full_duty = full_duty;
    servos[servo].min_angle = min_angle;
    servos[servo].max_angle = max_angle;

    // precalculate the ranges for efficiency when setting the angle
    servos[servo].duty_range = full_duty - zero_duty;
    servos[servo].angle_range = max_angle - min_angle;

    gpio_set_function(servos[servo].pin, GPIO_FUNC_PWM);
    servos[servo].slice_num = pwm_gpio_to_slice_num(servos[servo].pin);

    servos[servo].configured = true;

    uint32_t clock  = clock_get_hz(clk_sys);
    uint32_t divisor = clock / (20000 * 50); // for 50Hz

    pwm_config config = pwm_get_default_config();
    pwm_config_set_clkdiv(&config, (float)divisor);
    pwm_config_set_wrap(&config, 20000);
    pwm_init(servos[servo].slice_num, &config, false);

    servos[servo].reset = at_the_end_of_time;

    servo_start(servo, min_angle);

    servo_set_angle(servo, 90);
    busy_wait_ms(PERCUSSIVE_RETURN_MS);
    servo_set_angle(servo, 0);

    servos_in_use++;
}

/* servo_start initialises a servo
    @param s servo index into servos[] array
    @param start_angle initial angle in degrees */
void servo_start(const int s, uint16_t start_angle) {
    const uint chan = pwm_gpio_to_channel(servos[s].pin);
    uint16_t pulse = (start_angle - servos[s].min_angle) * servos[s].full_duty / servos[s].angle_range + servos[s].zero_duty;
    pwm_set_chan_level(servos[s].slice_num, chan, pulse);
    pwm_set_enabled(servos[s].slice_num, true);
}

void servo_stop(const int servo) {
    if ((servo < MAX_SERVOS) && servos[servo].configured) {
        pwm_set_enabled(servos[servo].slice_num, false);
    }
}

/* servo_set_angle moves a servo
    @param s servo index into servos[] array
    @param angle in degrees */
void servo_set_angle(const int s, uint16_t angle) {
    if ((s < MAX_SERVOS) && servos[s].configured) {
        const uint chan = pwm_gpio_to_channel(servos[s].pin);
        const uint16_t level = (angle - servos[s].min_angle) * servos[s].full_duty / servos[s].angle_range + servos[s].zero_duty;
        pwm_set_chan_level(servos[s].slice_num, chan, level);
    }
}

void servo_set_reset_time(int servo, absolute_time_t when) {
    servos[servo].reset = when;
}