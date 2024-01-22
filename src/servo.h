/**
 * SPDX-FileCopyrightText: 2023 Stephen Merrony
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef SERVO_H
#define SERVO_H

#include "pico/stdlib.h"

/* MAX_SERVOS is the max number of servos one (Pico) device can manage via PWM */
#define MAX_SERVOS 16

typedef struct {
    uint pin;
    uint slice_num;
    uint16_t zero_duty;
    uint16_t full_duty;
    uint16_t min_angle;
    uint16_t max_angle;
    uint16_t duty_range;    // calculated for efficiency
    uint16_t angle_range;   // calculated for efficiency
    absolute_time_t reset;
    bool configured;
} servo_t;

void resetter_task();

void servo_setup(int servo, uint pin, uint16_t zero_duty, uint16_t full_duty, uint16_t min_angle, uint16_t max_angle);
void servo_start(const int servo, uint16_t start_angle);
void servo_stop(const int servo);
void servo_set_angle(const int servo, uint16_t angle);
void servo_set_reset_time(int servo, absolute_time_t when);

#endif